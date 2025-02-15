# Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_minimum_required(VERSION 3.15.3)
project(as5600)

#
# Pathes
#
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH DRIVER_DIR)
cmake_path(GET DRIVER_DIR PARENT_PATH DRIVERS_DIR)
cmake_path(GET DRIVERS_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

#
# Config
#
set(APP_PLATFORM ubuntu)

#
# Dependencies
#
find_package(GTest REQUIRED)

#
# Includes
#
include(${DRIVER_DIR}/CMakeLists.txt)

#
# Target
#
set(EXECUTABLE test_${PROJECT_NAME})
add_executable(${EXECUTABLE}
    main.cpp
    ${APPLICATION_SOURCES}
    ${PERIPHERAL_SOURCES}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${GTEST_INCLUDE_DIRS}
    ../../../../Src
)

target_link_libraries(${EXECUTABLE}
    GTest::GTest
)
