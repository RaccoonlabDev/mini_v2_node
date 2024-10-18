# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${ROOT_DIR}/Src)
set(CMAKE_DIR ${ROOT_DIR}/cmake)
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(DOC_PATH ${ROOT_DIR}/docs/${CAN_PROTOCOL}/README.md)

# Option 1. Choose the platform
option(USE_PLATFORM_UBUNTU "Build for SITL (Software In The Loop)" OFF)
option(USE_PLATFORM_NODE_V2 "Build for stm32f103xB (128 KBytes)" OFF)
option(USE_PLATFORM_NODE_V3 "Build for stm32g0b1 (512 KBytes)" OFF)

if(USE_PLATFORM_UBUNTU)
    set(LIBPARAMS_PLATFORM ubuntu)
    set(CAN_PLATFORM socketcan)
    set(APP_PLATFORM ubuntu)
    set(PLATFORM_NAME sitl)
    elseif(USE_PLATFORM_NODE_V2)
    set(LIBPARAMS_PLATFORM stm32f103)
    set(CAN_PLATFORM bxcan)
    set(APP_PLATFORM stm32f103)
    set(PLATFORM_NAME v2)
    include(${CMAKE_DIR}/stm32f103xB.cmake)
elseif(USE_PLATFORM_NODE_V3)
    set(LIBPARAMS_PLATFORM stm32g0b1)
    set(CAN_PLATFORM fdcan)
    set(APP_PLATFORM stm32g0b1)
    set(PLATFORM_NAME v3)
    include(${CMAKE_DIR}/stm32g0b1.cmake)
else()
    message(SEND_ERROR "Platform Error: Either v2 (stm32f103), v3 (stm32g0) or SITL (Linux) should be specified.")
endif()

# Option 2. Choose the protocol
if(CAN_PROTOCOL STREQUAL "dronecan" OR CAN_PROTOCOL STREQUAL "cyphal" OR CAN_PROTOCOL STREQUAL "both")
    set(APPLICATION_DIR ${ROOT_DIR}/Src/applications/${CAN_PROTOCOL})
else()
    message(SEND_ERROR "CAN_PROTOCOL is unknown.")
endif()

# Set build dir based on hardware version and protocol
set(BUILD_ROOT_DIR ${ROOT_DIR}/build)
set(BUILD_TARGET_DIR ${BUILD_ROOT_DIR}/${CAN_PROTOCOL}_${PLATFORM_NAME})
set(BUILD_SRC_DIR ${BUILD_TARGET_DIR}/src)
set(BUILD_OBJ_DIR ${BUILD_TARGET_DIR}/obj)

if(USE_PLATFORM_UBUNTU)
    add_definitions(-DLIBPARAMS_PARAMS_DIR="${BUILD_SRC_DIR}")
    add_definitions(-DLIBPARAMS_INIT_PARAMS_FILE_NAME="default_params")
    add_definitions(-DLIBPARAMS_TEMP_PARAMS_FILE_NAME="temp_params")
endif()

include(${LIBPARAMS_PATH}/CMakeLists.txt)

list(APPEND APPLICATION_HEADERS ${ROOT_DIR}/Src ${APPLICATION_DIR})
include(${APPLICATION_DIR}/CMakeLists.txt)

# Set compile options
set(WARNING_FLAGS "-Wall -Wextra -Wfloat-equal -Werror -Wundef -Wshadow -Wpointer-arith -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Wno-volatile")
set(CMAKE_CXX_STANDARD 20)

# Prebuild
include(${CMAKE_DIR}/params.cmake)
include(${CMAKE_DIR}/git.cmake)

# Hardware version is not supported at the momemnt, let's hardcode it for a while
add_definitions(-DHW_VERSION_MAJOR=2)
add_definitions(-DHW_VERSION_MINOR=1)

# Project
project(example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(${CMAKE_DIR}/full_firmware_info.cmake)

add_subdirectory(${ROOT_DIR}/Src/platform/${APP_PLATFORM})
