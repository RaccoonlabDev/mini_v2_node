# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Function to ensure a variable is within the range of 0 to 99
function(check_version_component component_name component_value)
    if(NOT ("${component_value}" MATCHES "^[0-9]+$"))
        message(FATAL_ERROR "${component_name} must be an integer.")
    endif()

    if(${component_value} GREATER 99 OR ${component_value} LESS 0)
        message(FATAL_ERROR "${component_name} must be between 0 and 99. Current value: ${component_value}")
    endif()
endfunction()

function(resolve_version_component flag output_var)
    execute_process(
        COMMAND rl-git-info ${flag}
        WORKING_DIRECTORY ${ROOT_DIR}
        RESULT_VARIABLE _rl_git_info_result
        OUTPUT_VARIABLE _rl_git_info_output
        ERROR_VARIABLE _rl_git_info_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(_rl_git_info_result EQUAL 0)
        set(${output_var} "${_rl_git_info_output}" PARENT_SCOPE)
    else()
        set(${output_var} "0" PARENT_SCOPE)
    endif()
endfunction()

execute_process(
    COMMAND git rev-parse --short=16 HEAD
    WORKING_DIRECTORY ${ROOT_DIR}
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_VARIABLE GIT_HASH_SHORT_16_DIGITS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(SUBSTRING "${GIT_HASH_SHORT_16_DIGITS}" 0 8 GIT_HASH_SHORT_8_DIGITS)

resolve_version_component(--major APP_VERSION_MAJOR)
resolve_version_component(--minor APP_VERSION_MINOR)
resolve_version_component(--patch APP_VERSION_PATCH)

execute_process(
    COMMAND git log -1 --format=%cd --date=format:"%Y.%m.%d"
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE LATEST_COMMIT_DATE
    ERROR_QUIET
)

# Trim whitespace from the output variable
string(STRIP "${LATEST_COMMIT_DATE}" LATEST_COMMIT_DATE)

# Check each version component
check_version_component("APP_VERSION_MAJOR" ${APP_VERSION_MAJOR})
check_version_component("APP_VERSION_MINOR" ${APP_VERSION_MINOR})
check_version_component("APP_VERSION_PATCH" ${APP_VERSION_PATCH})

add_definitions(-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR})
add_definitions(-DAPP_VERSION_MINOR=${APP_VERSION_MINOR})
add_definitions(-DAPP_VERSION_PATCH=${APP_VERSION_PATCH})

set(GIT_HASH "0x${GIT_HASH_SHORT_16_DIGITS}")
add_definitions(-DGIT_HASH=${GIT_HASH})
