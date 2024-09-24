# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT CMAKE_CXX_COMPILER_ID)
    message(FATAL_ERROR "C++ compiler not set yet")
endif()
set(TOOLCHAIN_INFO "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
add_definitions(-DFIRMWARE_FULL_INFO="Node v${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}_${GIT_HASH_SHORT_8_DIGITS} ${CMAKE_BUILD_TYPE} ${TOOLCHAIN_INFO}")
