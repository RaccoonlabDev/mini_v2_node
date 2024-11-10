# Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_minimum_required(VERSION 3.10)
project(as5600)

add_library(as5600
    ../as5600.cpp
    ../../Src/platform/ubuntu/i2c.cpp
)

target_include_directories(as5600 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ../../../../Src
)

add_executable(test_as5600
    test_as5600.cpp
)

target_link_libraries(test_as5600
    as5600
    GTest::GTest
    GTest::Main
)
