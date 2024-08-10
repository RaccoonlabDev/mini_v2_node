# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)

include(${SRC_DIR}/modules/dronecan_module/CMakeLists.txt)
include(${APPLICATION_DIR}/modules/circuit_status/CMakeLists.txt)
include(${APPLICATION_DIR}/modules/pwm/CMakeLists.txt)
