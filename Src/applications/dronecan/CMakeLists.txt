# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH APPLICATIONS_DIR)
cmake_path(GET APPLICATIONS_DIR PARENT_PATH SRC_DIR)

include(${SRC_DIR}/modules/dronecan/core/CMakeLists.txt)
include(${SRC_DIR}/modules/dronecan/circuit_status/CMakeLists.txt)
include(${SRC_DIR}/modules/dronecan/pwm/CMakeLists.txt)

if(USE_PLATFORM_NODE_V3)
    include(${SRC_DIR}/modules/imu/CMakeLists.txt)
endif()
