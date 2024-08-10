# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH APPLICATIONS_DIR)
cmake_path(GET APPLICATIONS_DIR PARENT_PATH SRC_DIR)
set(MODULES_DIR ${SRC_DIR}/modules)

include(${MODULES_DIR}/cyphal/core/CMakeLists.txt)
include(${MODULES_DIR}/cyphal/circuit_status/CMakeLists.txt)
include(${MODULES_DIR}/cyphal/feedback/CMakeLists.txt)
include(${MODULES_DIR}/cyphal/setpoint/CMakeLists.txt)
