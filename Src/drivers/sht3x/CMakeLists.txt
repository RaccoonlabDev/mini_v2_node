# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Pathes
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH DRIVERS_DIR)
cmake_path(GET DRIVERS_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

# Include guard
if(SRC_DRIVERS_SHT3X_DIR)
    return()
endif()
set(SRC_DRIVERS_SHT3X_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_SOURCES
    ${ROOT_DIR}/Src/drivers/sht3x/sht3x.cpp
)

include(${ROOT_DIR}/Src/peripheral/i2c/CMakeLists.txt)
