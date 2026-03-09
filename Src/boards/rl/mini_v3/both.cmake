# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM stm32g0b1)
set(STM32_CUBEMX_PROJECT_PATH ${ROOT_DIR}/Libs/mini-v3-ioc)
set(CAN_PROTOCOL both)
if(BOARD_CONFIG_ONLY)
    return()
endif()
include(${CMAKE_CURRENT_LIST_DIR}/cyphal.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dronecan.cmake)
