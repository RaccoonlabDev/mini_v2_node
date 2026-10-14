# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# An example, not a product feature: the FDCAN driver of libdcnode (stm32g0b1, stm32h753xx) sends
# every frame with a 29-bit id, so there an 11-bit frame of this module would look like DroneCAN.
if(NOT APP_PLATFORM STREQUAL "ubuntu" AND NOT APP_PLATFORM STREQUAL "stm32f103")
    message(FATAL_ERROR "backends/raw_can/board_status needs 11-bit CAN ids: SITL and stm32f103 only.")
endif()

rl_include_module(board_status)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/board_status.cpp
)
