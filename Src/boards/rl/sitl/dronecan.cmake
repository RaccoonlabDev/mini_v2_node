# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM ubuntu)
if(NOT DEFINED CAN_PROTOCOL OR CAN_PROTOCOL STREQUAL "")
    set(CAN_PROTOCOL dronecan)
endif()
if(BOARD_CONFIG_ONLY)
    return()
endif()
add_definitions(-DCONFIG_USE_DRONECAN=1)

include(${ROOT_DIR}/Src/modules/dronecan/core/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/application/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/system/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/dronecan/arming/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/circuit_status/dronecan/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/feedback/dronecan/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/rcout/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/sitl/CMakeLists.txt)

if(USE_PLATFORM_NODE_V3 OR USE_PLATFORM_UBUNTU)
    include(${ROOT_DIR}/Src/modules/imu/CMakeLists.txt)
endif()
