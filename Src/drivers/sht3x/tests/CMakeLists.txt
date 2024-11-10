# Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_minimum_required(VERSION 3.10)
project(sht3x)

add_library(sht3x
    ../sht3x.cpp
    ../../../Src/platform/ubuntu/i2c.cpp
)

target_include_directories(sht3x PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ../../../../Src
)

add_executable(test_sht3x
    test_sht3x.cpp
)

target_link_libraries(test_sht3x
    sht3x
    GTest::GTest
    GTest::Main
)
