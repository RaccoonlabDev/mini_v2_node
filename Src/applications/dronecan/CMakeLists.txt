# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH APPLICATIONS_DIR)
cmake_path(GET APPLICATIONS_DIR PARENT_PATH SRC_DIR)

add_definitions(-DCONFIG_USE_DRONECAN=1)

include(${SRC_DIR}/modules/dronecan/core/CMakeLists.txt)
include(${SRC_DIR}/modules/system/CMakeLists.txt)
include(${SRC_DIR}/modules/dronecan/arming/CMakeLists.txt)
include(${SRC_DIR}/modules/circuit_status/dronecan/CMakeLists.txt)
include(${SRC_DIR}/modules/feedback/dronecan/CMakeLists.txt)
include(${SRC_DIR}/modules/pwm/CMakeLists.txt)

if(USE_PLATFORM_NODE_V3)
    include(${SRC_DIR}/modules/imu/CMakeLists.txt)
endif()
