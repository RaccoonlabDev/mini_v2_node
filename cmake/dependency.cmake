# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# The functions behind cmake/dependencies.cmake, see README.md, Libraries.
include_guard(GLOBAL)

get_filename_component(_nc_root_dir "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
set_property(GLOBAL PROPERTY NC_ROOT_DIR "${_nc_root_dir}")
set(NC_THIRD_PARTY_DIR "${_nc_root_dir}/build/third_party" CACHE PATH
    "Where libraries are cloned, one directory per version")

# nc_declare_dependency(<name> <owner/repo> SUBMODULE <path> | VERSION <tag or commit>)
#
# Declares a library and its default source: a submodule of node-core, or a version cloned at
# configure time. Creates the cache variables an application overrides the default with:
#   NC_<NAME>_VERSION - a tag or commit to clone instead
#   NC_<NAME>_PATH    - a local checkout to use as is
function(nc_declare_dependency name repository kind default)
    if(NOT kind MATCHES "^(SUBMODULE|VERSION)$")
        message(FATAL_ERROR "nc_declare_dependency(${name}): expected SUBMODULE or VERSION, got ${kind}")
    endif()
    string(TOUPPER "${name}" upper)
    set_property(GLOBAL APPEND PROPERTY NC_DEPENDENCIES ${name})
    set_property(GLOBAL PROPERTY NC_DEPENDENCY_${name}_REPOSITORY ${repository})
    set_property(GLOBAL PROPERTY NC_DEPENDENCY_${name}_${kind} ${default})
    set(NC_${upper}_VERSION "" CACHE STRING "${name}: a tag or commit of ${repository} to build instead of the default")
    set(NC_${upper}_PATH "" CACHE PATH "${name}: a local checkout to build instead of the default")
endfunction()

# nc_use_dependency(<name>)
#
# Sets NC_<NAME>_PATH to the source of a declared library, cloning it when needed. The caller adds
# it to the build. The first one set wins: NC_<NAME>_PATH, NC_<NAME>_VERSION, the default.
function(nc_use_dependency name)
    string(TOUPPER "${name}" upper)
    get_property(resolved GLOBAL PROPERTY NC_DEPENDENCY_${name}_RESOLVED)
    if(resolved)
        set(NC_${upper}_PATH "${resolved}" PARENT_SCOPE)
        return()
    endif()

    get_property(repository GLOBAL PROPERTY NC_DEPENDENCY_${name}_REPOSITORY)
    get_property(submodule GLOBAL PROPERTY NC_DEPENDENCY_${name}_SUBMODULE)
    get_property(default_version GLOBAL PROPERTY NC_DEPENDENCY_${name}_VERSION)
    get_property(root_dir GLOBAL PROPERTY NC_ROOT_DIR)
    if(NOT repository)
        message(FATAL_ERROR "${name} is not declared in cmake/dependencies.cmake")
    endif()

    set(path "$CACHE{NC_${upper}_PATH}")
    set(version "$CACHE{NC_${upper}_VERSION}")
    # A build dir configured before these variables existed keeps the submodule as its path
    if(submodule AND path STREQUAL "${root_dir}/${submodule}")
        set(path "")
    endif()

    if(path AND version)
        message(FATAL_ERROR "Set NC_${upper}_PATH or NC_${upper}_VERSION, not both")
    elseif(path)
        if(NOT EXISTS "${path}/CMakeLists.txt")
            message(FATAL_ERROR "NC_${upper}_PATH=${path} is not a ${name} checkout")
        endif()
        set(origin "NC_${upper}_PATH")
    elseif(version)
        _nc_clone_dependency(${name} ${repository} ${version} path)
        set(origin "NC_${upper}_VERSION")
    elseif(submodule)
        set(path "${root_dir}/${submodule}")
        if(NOT EXISTS "${path}/CMakeLists.txt")
            message(FATAL_ERROR "${submodule} is empty, run: git submodule update --init --recursive")
        endif()
        set(origin "submodule")
    else()
        _nc_clone_dependency(${name} ${repository} ${default_version} path)
        set(origin "default")
    endif()

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${path}
        OUTPUT_VARIABLE commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT commit)
        set(commit "not a git checkout")
    endif()
    message(STATUS "${name}: ${path} (${commit}, ${origin})")
    set_property(GLOBAL PROPERTY NC_DEPENDENCY_${name}_RESOLVED "${path}")
    set(NC_${upper}_PATH "${path}" PARENT_SCOPE)
endfunction()

# Clones a version into its own directory, NC_THIRD_PARTY_DIR/<name>-<version>, unless it is there
# already. A clone is never updated: switching versions needs no fetch and can't mix two of them.
function(_nc_clone_dependency name repository version out_var)
    string(REGEX REPLACE "[^A-Za-z0-9._-]" "_" dir_suffix "${version}")
    set(dir "${NC_THIRD_PARTY_DIR}/${name}-${dir_suffix}")
    set(${out_var} "${dir}" PARENT_SCOPE)

    # Builds configured in parallel share the directory, so only one of them clones
    file(MAKE_DIRECTORY "${NC_THIRD_PARTY_DIR}")
    file(LOCK "${dir}.lock" GUARD FUNCTION TIMEOUT 600)
    if(IS_DIRECTORY "${dir}")
        return()
    endif()

    message(STATUS "Cloning ${repository} ${version} into ${dir}")
    set(tmp "${dir}.tmp")
    set(errors "")
    # HTTPS needs no key for a public repository; a private one falls back to the SSH key
    foreach(url https://github.com/${repository}.git git@github.com:${repository}.git)
        file(REMOVE_RECURSE "${tmp}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E env GIT_TERMINAL_PROMPT=0 GCM_INTERACTIVE=never
                    git clone --quiet ${url} ${tmp}
            RESULT_VARIABLE result
            ERROR_VARIABLE error
        )
        if(result EQUAL 0)
            break()
        endif()
        string(APPEND errors "  ${url}: ${error}")
    endforeach()
    if(NOT result EQUAL 0)
        file(REMOVE_RECURSE "${tmp}")
        string(TOUPPER "${name}" upper)
        message(FATAL_ERROR "Could not clone ${repository}:\n${errors}"
            "A private repository needs your SSH key. Without one (e.g. in Docker), "
            "run `make deps` on the host first or set NC_${upper}_PATH to a checkout.")
    endif()

    execute_process(
        COMMAND git -c advice.detachedHead=false checkout --quiet ${version}
        WORKING_DIRECTORY ${tmp}
        RESULT_VARIABLE result
        ERROR_VARIABLE error
    )
    if(NOT result EQUAL 0)
        file(REMOVE_RECURSE "${tmp}")
        message(FATAL_ERROR "${version} is not a tag or commit of ${repository}: ${error}")
    endif()
    execute_process(
        COMMAND git show-ref --verify --quiet refs/remotes/origin/${version}
        WORKING_DIRECTORY ${tmp}
        RESULT_VARIABLE branch_check
    )
    if(branch_check EQUAL 0)
        message(WARNING "${version} is a branch of ${repository}: ${dir} stays at its current "
            "commit. Pin a tag or a commit instead.")
    endif()

    execute_process(
        COMMAND git submodule update --init --recursive --quiet
        WORKING_DIRECTORY ${tmp}
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        file(REMOVE_RECURSE "${tmp}")
        message(FATAL_ERROR "Could not fetch the submodules of ${repository} ${version}")
    endif()

    file(RENAME "${tmp}" "${dir}")
endfunction()
