# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
cmake_minimum_required(VERSION 3.15.3)

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${ROOT_DIR}/Src)
set(CMAKE_DIR ${ROOT_DIR}/cmake)
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(LIBPARAMS_CMAKE libparams.cmake)
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
    include(${CMAKE_DIR}/toolchain-ubuntu.cmake)
elseif(USE_PLATFORM_NODE_V2)
    set(LIBPARAMS_PLATFORM stm32f103)
    set(CAN_PLATFORM bxcan)
    set(APP_PLATFORM stm32f103)
    set(PLATFORM_NAME v2)

    set(stm32cubeMxProjectPath  ${ROOT_DIR}/Libs/stm32-cube-project)
    FILE(GLOB ldFile            ${stm32cubeMxProjectPath}/*_FLASH.ld)
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c)
    FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                                ${stm32cubeMxProjectPath}/Core/Startup/*.s
    )

    set(CMAKE_EXE_LINKER_FLAGS "-T${ldFile} -Wl,-Map=${PROJECT_NAME}.map,--cref")
    include(${CMAKE_DIR}/toolchain-stm32f103xB.cmake)
elseif(USE_PLATFORM_NODE_V3)
    set(LIBPARAMS_PLATFORM stm32g0b1)
    set(CAN_PLATFORM fdcan)
    set(APP_PLATFORM stm32g0b1)
    set(PLATFORM_NAME v3)

    set(stm32cubeMxProjectPath  ${ROOT_DIR}/Libs/mini-v3-ioc)
    FILE(GLOB ldFile            ${stm32cubeMxProjectPath}/*_FLASH.ld)
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c)
    FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                                ${stm32cubeMxProjectPath}/Core/Startup/*.s
    )

    set(CMAKE_EXE_LINKER_FLAGS "-T${ldFile} -Wl,-Map=${PROJECT_NAME}.map,--cref")
    include(${CMAKE_DIR}/toolchain-stm32g0b1.cmake)
    add_compile_definitions(CYPHAL_NUM_OF_CAN_BUSES=2)
else()
    message(SEND_ERROR "Platform Error: Either v2 (stm32f103), v3 (stm32g0) or SITL (Linux) should be specified.")
endif()

# Option 2. Choose the protocol
set(APPLICATION_DIR ${ROOT_DIR}/Src/applications/${CAN_PROTOCOL})
if(NOT (EXISTS ${APPLICATION_DIR} AND IS_DIRECTORY ${APPLICATION_DIR}))
    message(FATAL_ERROR "CAN_PROTOCOL '${CAN_PROTOCOL}' is unknown.")
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

include(${LIBPARAMS_PATH}/${LIBPARAMS_CMAKE})

list(APPEND APPLICATION_HEADERS ${ROOT_DIR}/Src ${APPLICATION_DIR})
include(${APPLICATION_DIR}/CMakeLists.txt)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode is enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -DDEBUG")
endif()

# Prebuild
include(${CMAKE_DIR}/params.cmake)
include(${CMAKE_DIR}/git.cmake)

# Hardware version is not supported at the momemnt, let's hardcode it for a while
add_definitions(-DHW_VERSION_MAJOR=2)
add_definitions(-DHW_VERSION_MINOR=1)

# Project
project(node CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(${CMAKE_DIR}/full_firmware_info.cmake)

include(${ROOT_DIR}/Src/peripheral/adc/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/gpio/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/iwdg/CMakeLists.txt)

add_subdirectory(${ROOT_DIR}/Src/platform/${APP_PLATFORM})
