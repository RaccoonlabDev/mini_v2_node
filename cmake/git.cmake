# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

execute_process(
    COMMAND git rev-parse --short=16 HEAD
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_VARIABLE GIT_HASH_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND rl-git-info --major
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_VARIABLE APP_VERSION_MAJOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND rl-git-info --minor
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_VARIABLE APP_VERSION_MINOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR})
add_definitions(-DAPP_VERSION_MINOR=${APP_VERSION_MINOR})

set(GIT_HASH "0x${GIT_HASH_SHORT}")
add_definitions(-DGIT_HASH=${GIT_HASH})
