# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(SRC_DRIVERS_INA228_DIR)
    return()
endif()
set(SRC_DRIVERS_INA228_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ina228.cpp
)

include(${ROOT_DIR}/Src/peripheral/i2c/CMakeLists.txt)
