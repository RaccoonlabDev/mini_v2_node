# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# This frontend keeps the place and the VSSC bit of the former circuit_status module.
rl_include_module(board_status)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/board_status.cpp
)

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)
