# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(SRC_DRIVERS_MPU9250_DIR)
    return()
endif()
set(SRC_DRIVERS_MPU9250_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/mpu9250.cpp
)

include(${ROOT_DIR}/Src/peripheral/spi/CMakeLists.txt)
