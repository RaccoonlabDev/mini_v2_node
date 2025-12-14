# Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# use_external_github_library.cmake
#
# Function:
#   use_external_github_library(<LIBRARY_NAME> <LIBRARY_VERSION> <GITHUB_REPO> <BUILD_FROM_SOURCE>)
#
# Parameters:
#   <LIBRARY_NAME>        Logical name of the library (e.g., DronecanSimInterface)
#   <FETCH_METHOD>        There are a few ways to fetch the library:
#                         - "source" to build from source
#                         - "prebuild" to use a precompiled release
#                         - "submodule" to use a submodule
#   <GITHUB_REPO>         GitHub repo in org/name format (e.g., PonomarevDA/DronecanSimInterface)
#   <LIBRARY_VERSION>     Semantic version or release tag (e.g., 0.1.0)
#   <DOWNLOAD_DIR>        The directory where "source" or "prebuild" is downloaded.
#                         For example: ${CMAKE_BINARY_DIR}/downloads
#   <EXTRACT_DIR>         The directory where "source" or "prebuild" is downloaded.
#                         For example: ${CMAKE_BINARY_DIR}/externals

function(use_external_github_library
    LIBRARY_NAME
    FETCH_METHOD
    GITHUB_REPO
    LIBRARY_VERSION
)
    set(options "")
    set(one_value_args DOWNLOAD_DIR EXTRACT_DIR)
    set(multi_value_args "")

    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(FETCH_METHOD STREQUAL "source")
        message(STATUS "Using ${LIBRARY_NAME} from source: ${GITHUB_REPO}")
        set(SSH_LINK git@github.com:${GITHUB_REPO}.git)
        set(HTTPS_LINK https://github.com/${GITHUB_REPO}.git)

        if(NOT ARG_DOWNLOAD_DIR)
            if(DOWNLOAD_DIR)
                set(ARG_DOWNLOAD_DIR ${DOWNLOAD_DIR})
            else()
                message(FATAL_ERROR "DOWNLOAD_DIR must be specified")
            endif()
        endif()

        set(REPO_DOWNLOAD_DIR ${ARG_DOWNLOAD_DIR}/${LIBRARY_NAME})

        if(NOT EXISTS "${REPO_DOWNLOAD_DIR}/.git")
            execute_process(
                COMMAND git clone --recursive ${SSH_LINK} ${REPO_DOWNLOAD_DIR}
                RESULT_VARIABLE CLONE_RESULT
            )
            if(NOT CLONE_RESULT EQUAL 0)
                message(WARNING "Failed to clone ${SSH_LINK}. Trying HTTPS...")
                execute_process(
                    COMMAND git clone --recursive ${HTTPS_LINK} ${REPO_DOWNLOAD_DIR}
                    RESULT_VARIABLE CLONE_RESULT
                )
                if(NOT CLONE_RESULT EQUAL 0)
                    message(FATAL_ERROR "Failed to clone ${SSH_LINK}.")
                endif()
            endif()
        endif()

        execute_process(
            COMMAND git checkout ${LIBRARY_VERSION}
            WORKING_DIRECTORY ${REPO_DOWNLOAD_DIR}
            RESULT_VARIABLE CHECKOUT_RESULT
        )
        if(NOT CHECKOUT_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to checkout ${LIBRARY_VERSION} in ${REPO_DOWNLOAD_DIR}")
        endif()

        add_subdirectory(${REPO_DOWNLOAD_DIR} ${CMAKE_BINARY_DIR}/${LIBRARY_NAME})

    elseif(FETCH_METHOD STREQUAL "prebuild")
        message(STATUS "Using ${LIBRARY_NAME} precompiled from release: ${GITHUB_REPO}")

        if(NOT ARG_DOWNLOAD_DIR)
            if(DOWNLOAD_DIR)
                set(ARG_DOWNLOAD_DIR ${DOWNLOAD_DIR})
            else()
                message(FATAL_ERROR "DOWNLOAD_DIR must be specified")
            endif()
        endif()

        if(NOT ARG_EXTRACT_DIR)
            if(EXTRACT_DIR)
                set(ARG_EXTRACT_DIR ${EXTRACT_DIR})
            else()
                message(FATAL_ERROR "EXTRACT_DIR must be specified")
            endif()
        endif()

        set(TAR_GZ_NAME ${LIBRARY_NAME}-${LIBRARY_VERSION}.tar.gz)
        set(TAR_GZ_URL "https://github.com/${GITHUB_REPO}/releases/download/${LIBRARY_VERSION}/${TAR_GZ_NAME}")
        set(DOWNLOAD_TAG_GZ_PATH "${ARG_DOWNLOAD_DIR}/${TAR_GZ_NAME}")

        if(NOT EXISTS "${DOWNLOAD_TAG_GZ_PATH}")
            find_program(GH_CMD gh)
            if(GH_CMD)
                message(STATUS "Fetching from GitHub: ${TAR_GZ_URL}")
                execute_process(
                    COMMAND gh release download ${LIBRARY_VERSION}
                            --repo ${GITHUB_REPO}
                            --pattern ${TAR_GZ_NAME}
                            --output "${DOWNLOAD_TAG_GZ_PATH}"
                    RESULT_VARIABLE DOWNLOAD_RESULT
                    ERROR_VARIABLE DOWNLOAD_ERR
                )
                if(NOT DOWNLOAD_RESULT EQUAL 0)
                    message(WARNING "Failed to download ${LIBRARY_NAME} archive (exit ${DOWNLOAD_RESULT}):\n ${DOWNLOAD_ERR}. Trying HTTPS...")
                    file(DOWNLOAD "${TAR_GZ_URL}" "${DOWNLOAD_TAG_GZ_PATH}" SHOW_PROGRESS TLS_VERIFY ON)
                endif()
            else()
                message(WARNING "GitHub CLI not found! "
                                "On Ubuntu: sudo snap install gh. "
                                "See https://cli.github.com/manual/installation for other platforms. "
                                "Trying HTTPS..."
                )
                file(DOWNLOAD "${TAR_GZ_URL}" "${DOWNLOAD_TAG_GZ_PATH}" SHOW_PROGRESS TLS_VERIFY ON)
            endif()
        endif()

        file(MAKE_DIRECTORY "${ARG_EXTRACT_DIR}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${DOWNLOAD_TAG_GZ_PATH}"
            WORKING_DIRECTORY "${ARG_EXTRACT_DIR}"
        )

        list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/external")
        find_package(${LIBRARY_NAME} REQUIRED)
        if(NOT ${LIBRARY_NAME}_FOUND)
            message(FATAL_ERROR "${LIBRARY_NAME} not found after extract")
        endif()

    elseif(FETCH_METHOD STREQUAL "submodule")
        add_subdirectory(${GITHUB_REPO} ${CMAKE_BINARY_DIR}/${LIBRARY_NAME})

    else()
        message(FATAL_ERROR "Unknown fetch method!")

    endif()

endfunction()
