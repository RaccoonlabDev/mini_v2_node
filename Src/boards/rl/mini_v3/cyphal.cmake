# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM stm32g0b1)
set(STM32_CUBEMX_PROJECT_PATH ${ROOT_DIR}/Libs/mini-v3-ioc)
if(NOT DEFINED CAN_PROTOCOL OR CAN_PROTOCOL STREQUAL "")
    set(CAN_PROTOCOL cyphal)
endif()
if(BOARD_CONFIG_ONLY)
    return()
endif()
add_definitions(-DCONFIG_USE_CYPHAL=1)
add_definitions(-DLIBCPNODE_CUSTOM_HEAP_SIZE=4096)

include(${ROOT_DIR}/Src/modules/cyphal/core/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/system/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/circuit_status/cyphal/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/feedback/cyphal/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/rcout/CMakeLists.txt)
