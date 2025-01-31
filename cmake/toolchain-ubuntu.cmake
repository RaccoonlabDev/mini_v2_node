# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_minimum_required(VERSION 3.15.3)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ==================================
#  Target System
# ==================================
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ==================================
#  Compilation and Linker Flags
# ==================================
set(WARNING_FLAGS "-Wall -Wextra -Wfloat-equal -Werror -Wundef -Wshadow -Wpointer-arith -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default")  # these could be also useful: -Wconversion -Wsign-conversion

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${COMMON_FLAGS} ${WARNING_FLAGS}")  # these could be also useful: -Wmissing-prototypes -Wstrict-prototypes
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Wno-volatile")

# ==================================
#  Language Standards
# ==================================
if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
endif()
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
