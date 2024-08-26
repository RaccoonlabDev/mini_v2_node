# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH MODULES_DIR)
# cmake_path(GET MODULES_DIR PARENT_PATH SRC_DIR)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/imu.cpp
)

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)

include(${SRC_DIR}/drivers/mpu9250/CMakeLists.txt)
