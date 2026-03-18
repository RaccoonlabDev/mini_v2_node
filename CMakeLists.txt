# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
cmake_minimum_required(VERSION 3.15.3)

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${ROOT_DIR}/Src)
set(CMAKE_DIR ${ROOT_DIR}/cmake)
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(LIBPARAMS_CMAKE libparams.cmake)
set(BUILD_ROOT_DIR ${ROOT_DIR}/build)

include(${CMAKE_DIR}/module.cmake)

# Option 1. Choose board and target
set(NC_BOARD "" CACHE STRING "Board path under Src/boards or app boards in form vendor/name")
set(NC_TARGET "" CACHE STRING "Board target file name without extension")
set(NC_IMAGE_KIND "" CACHE STRING "Firmware image kind: standalone/application")
set(BOARD "" CACHE STRING "Deprecated. Use NC_BOARD.")
set(BOARD_TARGET "" CACHE STRING "Deprecated. Use NC_TARGET.")
set(BOARD_FILE "" CACHE FILEPATH "Optional explicit path to board metadata file")
set(BOARD_TARGET_FILE "" CACHE FILEPATH "Optional explicit path to board target file")
set(IMAGE_KIND "" CACHE STRING "Deprecated. Use NC_IMAGE_KIND.")
option(USE_PLATFORM_UBUNTU "Build for SITL (Software In The Loop)" OFF)
option(USE_PLATFORM_NODE_V2 "Build for stm32f103xB (128 KBytes)" OFF)
option(USE_PLATFORM_NODE_V3 "Build for stm32g0b1 (512 KBytes)" OFF)
set(BOARD_VENDOR rl CACHE STRING "Board vendor name under Src/boards")

if(NC_BOARD STREQUAL "" AND NOT BOARD STREQUAL "")
    set(NC_BOARD "${BOARD}")
endif()
if(BOARD STREQUAL "" AND NOT NC_BOARD STREQUAL "")
    set(BOARD "${NC_BOARD}")
endif()
if(NC_TARGET STREQUAL "" AND NOT BOARD_TARGET STREQUAL "")
    set(NC_TARGET "${BOARD_TARGET}")
endif()
if(BOARD_TARGET STREQUAL "" AND NOT NC_TARGET STREQUAL "")
    set(BOARD_TARGET "${NC_TARGET}")
endif()
if(NC_IMAGE_KIND STREQUAL "" AND NOT IMAGE_KIND STREQUAL "")
    set(NC_IMAGE_KIND "${IMAGE_KIND}")
endif()
if(IMAGE_KIND STREQUAL "" AND NOT NC_IMAGE_KIND STREQUAL "")
    set(IMAGE_KIND "${NC_IMAGE_KIND}")
endif()

if(NC_BOARD STREQUAL "")
    message(FATAL_ERROR "NC_BOARD is not specified. Use -DNC_BOARD=<vendor>/<name>.")
endif()

if(NC_TARGET STREQUAL "")
    if(NOT "${CAN_PROTOCOL}" STREQUAL "")
        set(NC_TARGET "${CAN_PROTOCOL}")
    else()
        set(NC_TARGET "default")
    endif()
endif()
set(BOARD_TARGET "${NC_TARGET}")

string(REPLACE "/" ";" BOARD_PARTS "${NC_BOARD}")
list(LENGTH BOARD_PARTS BOARD_PARTS_LENGTH)
if(NOT BOARD_PARTS_LENGTH EQUAL 2)
    message(FATAL_ERROR "NC_BOARD must be in form <vendor>/<name>, got '${NC_BOARD}'.")
endif()
list(GET BOARD_PARTS 0 BOARD_VENDOR)
list(GET BOARD_PARTS 1 BOARD_NAME)
set(CORE_BOARD_DIR ${ROOT_DIR}/Src/boards/${BOARD_VENDOR}/${BOARD_NAME})
set(APP_BOARD_DIR "")
if(NOT "${APP_BOARDS_DIR}" STREQUAL "")
    set(APP_BOARD_DIR ${APP_BOARDS_DIR}/${BOARD_VENDOR}/${BOARD_NAME})
endif()

set(BOARD_DIR "")
if(NOT "${APP_BOARD_DIR}" STREQUAL "" AND EXISTS "${APP_BOARD_DIR}" AND IS_DIRECTORY "${APP_BOARD_DIR}")
    set(BOARD_DIR ${APP_BOARD_DIR})
elseif(EXISTS "${CORE_BOARD_DIR}" AND IS_DIRECTORY "${CORE_BOARD_DIR}")
    set(BOARD_DIR ${CORE_BOARD_DIR})
endif()

# Option 2. Choose the board target
if(BOARD_TARGET_FILE STREQUAL "")
    if("${BOARD_DIR}" STREQUAL "")
        message(FATAL_ERROR "Board directory for '${NC_BOARD}' is not found in app or node-core.")
    endif()
    set(BOARD_TARGET_FILE ${BOARD_DIR}/${NC_TARGET}.cmake)
endif()
if(NOT EXISTS "${BOARD_TARGET_FILE}")
    message(FATAL_ERROR "Board target file '${BOARD_TARGET_FILE}' is not found.")
endif()

if(BOARD_FILE STREQUAL "")
    if("${BOARD_DIR}" STREQUAL "")
        message(FATAL_ERROR "Board directory for '${NC_BOARD}' is not found in app or node-core.")
    endif()
    set(BOARD_FILE ${BOARD_DIR}/board/board.cmake)
endif()
if(NOT EXISTS "${BOARD_FILE}")
    message(FATAL_ERROR "Board metadata file '${BOARD_FILE}' is not found.")
endif()

get_filename_component(BOARD_FILE_DIR ${BOARD_FILE} DIRECTORY)
get_filename_component(BOARD_DIR_FROM_FILE ${BOARD_FILE_DIR} DIRECTORY)
if(NOT (EXISTS "${BOARD_DIR_FROM_FILE}" AND IS_DIRECTORY "${BOARD_DIR_FROM_FILE}"))
    message(FATAL_ERROR "Derived board directory '${BOARD_DIR_FROM_FILE}' is not found.")
endif()

get_filename_component(BOARD_DIR_FROM_TARGET ${BOARD_TARGET_FILE} DIRECTORY)
if(NOT (EXISTS "${BOARD_DIR_FROM_TARGET}" AND IS_DIRECTORY "${BOARD_DIR_FROM_TARGET}"))
    message(FATAL_ERROR "Derived board directory '${BOARD_DIR_FROM_TARGET}' is not found.")
endif()

if(NOT BOARD_DIR_FROM_FILE STREQUAL BOARD_DIR_FROM_TARGET)
    message(FATAL_ERROR
        "BOARD_FILE and BOARD_TARGET_FILE refer to different board directories: "
        "'${BOARD_DIR_FROM_FILE}' vs '${BOARD_DIR_FROM_TARGET}'.")
endif()

set(BOARD_DIR ${BOARD_DIR_FROM_FILE})
include(${BOARD_FILE})

set(BOARD_TARGET_CONFIG_FILE ${BOARD_DIR}/board/${NC_TARGET}.cmake)
if(EXISTS "${BOARD_TARGET_CONFIG_FILE}")
    include(${BOARD_TARGET_CONFIG_FILE})
endif()

if(NOT DEFINED BOARD_PLATFORM)
    message(FATAL_ERROR "BOARD_PLATFORM is not set by board metadata for '${NC_BOARD}'.")
endif()

set(USE_PLATFORM_UBUNTU OFF)
set(USE_PLATFORM_NODE_V2 OFF)
set(USE_PLATFORM_NODE_V3 OFF)

# Keep these values aligned with `Src/common/mcu_family_id.hpp`.
# Reserved IDs:
#   0 = Unknown
#   1 = STM32F103
#   2 = STM32G0B1
#   3 = Ubuntu/SITL
#   4 = STM32H7
set(MCU_FAMILY_ID_UNKNOWN 0)
set(MCU_FAMILY_ID_STM32F103 1)
set(MCU_FAMILY_ID_STM32G0B1 2)
set(MCU_FAMILY_ID_UBUNTU 3)
set(MCU_FAMILY_ID_STM32H7 4)

if(BOARD_PLATFORM STREQUAL "ubuntu")
    set(LIBPARAMS_PLATFORM ubuntu)
    set(CAN_PLATFORM socketcan)
    set(LIBCPNODE_CAN_PLATFORM socketcan)
    set(APP_PLATFORM ubuntu)
    set(PLATFORM_NAME sitl)
    set(MCU_FAMILY_ID ${MCU_FAMILY_ID_UBUNTU})
    set(USE_PLATFORM_UBUNTU ON)
    include(${CMAKE_DIR}/toolchain-ubuntu.cmake)
    add_compile_definitions(USE_PLATFORM_UBUNTU=1)
elseif(BOARD_PLATFORM STREQUAL "stm32f103")
    set(LIBPARAMS_PLATFORM stm32f103)
    set(CAN_PLATFORM bxcan)
    set(LIBCPNODE_CAN_PLATFORM bxcan)
    set(APP_PLATFORM stm32f103)
    set(PLATFORM_NAME v2)
    set(MCU_FAMILY_ID ${MCU_FAMILY_ID_STM32F103})
    set(USE_PLATFORM_NODE_V2 ON)

    if(DEFINED STM32_CUBEMX_PROJECT_PATH)
        set(stm32cubeMxProjectPath  ${STM32_CUBEMX_PROJECT_PATH})
    else()
        set(stm32cubeMxProjectPath  ${ROOT_DIR}/Libs/stm32-cube-project)
    endif()
    if(DEFINED STM32_LINKER_SCRIPT_PATH)
        set(ldFile ${STM32_LINKER_SCRIPT_PATH})
    elseif(DEFINED STM32_LINKER_SCRIPT_TEMPLATE_PATH)
        if(NOT DEFINED LINK_FLASH_ORIGIN_HEX OR NOT DEFINED LINK_FLASH_LENGTH_KB)
            message(FATAL_ERROR "Linker template requires LINK_FLASH_ORIGIN_HEX and LINK_FLASH_LENGTH_KB.")
        endif()
        set(GENERATED_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/generated_linker.ld)
        configure_file(${STM32_LINKER_SCRIPT_TEMPLATE_PATH} ${GENERATED_LINKER_SCRIPT} @ONLY)
        set(ldFile ${GENERATED_LINKER_SCRIPT})
    else()
        FILE(GLOB ldFile        ${stm32cubeMxProjectPath}/*_FLASH.ld)
    endif()
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c
                                ${stm32cubeMxProjectPath}/Drivers/*/*/*/*.c)
    FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                                ${stm32cubeMxProjectPath}/Core/Startup/*.s
    )

    set(CMAKE_EXE_LINKER_FLAGS "-T${ldFile} -Wl,-Map=${PROJECT_NAME}.map,--cref")
    include(${CMAKE_DIR}/toolchain-stm32f103xB.cmake)
elseif(BOARD_PLATFORM STREQUAL "stm32g0b1")
    set(LIBPARAMS_PLATFORM stm32g0b1)
    set(CAN_PLATFORM fdcan)
    set(LIBCPNODE_CAN_PLATFORM fdcan)
    set(APP_PLATFORM stm32g0b1)
    set(PLATFORM_NAME v3)
    set(MCU_FAMILY_ID ${MCU_FAMILY_ID_STM32G0B1})
    set(USE_PLATFORM_NODE_V3 ON)

    if(DEFINED STM32_CUBEMX_PROJECT_PATH)
        set(stm32cubeMxProjectPath  ${STM32_CUBEMX_PROJECT_PATH})
    else()
        set(stm32cubeMxProjectPath  ${ROOT_DIR}/Libs/mini-v3-ioc)
    endif()
    if(DEFINED STM32_LINKER_SCRIPT_PATH)
        set(ldFile ${STM32_LINKER_SCRIPT_PATH})
    elseif(DEFINED STM32_LINKER_SCRIPT_TEMPLATE_PATH)
        if(NOT DEFINED LINK_FLASH_ORIGIN_HEX OR NOT DEFINED LINK_FLASH_LENGTH_KB)
            message(FATAL_ERROR "Linker template requires LINK_FLASH_ORIGIN_HEX and LINK_FLASH_LENGTH_KB.")
        endif()
        set(GENERATED_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/generated_linker.ld)
        configure_file(${STM32_LINKER_SCRIPT_TEMPLATE_PATH} ${GENERATED_LINKER_SCRIPT} @ONLY)
        set(ldFile ${GENERATED_LINKER_SCRIPT})
    else()
        FILE(GLOB ldFile        ${stm32cubeMxProjectPath}/*_FLASH.ld)
    endif()
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c
                                ${stm32cubeMxProjectPath}/Drivers/*/*/*/*.c)
    FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                                ${stm32cubeMxProjectPath}/Core/Startup/*.s
    )

    set(CMAKE_EXE_LINKER_FLAGS "-T${ldFile} -Wl,-Map=${PROJECT_NAME}.map,--cref")
    include(${CMAKE_DIR}/toolchain-stm32g0b1.cmake)
    add_compile_definitions(CYPHAL_NUM_OF_CAN_BUSES=2)
else()
    message(FATAL_ERROR "Unknown BOARD_PLATFORM '${BOARD_PLATFORM}'. Expected: ubuntu, stm32f103, stm32g0b1.")
endif()

add_definitions(-DMCU_FAMILY_ID=${MCU_FAMILY_ID})

include(${CMAKE_DIR}/git.cmake)

# Set build dir based on selected out-of-source build directory.
get_filename_component(BUILD_TARGET_DIR ${CMAKE_BINARY_DIR} DIRECTORY)
set(BUILD_SRC_DIR ${BUILD_TARGET_DIR}/src)
set(BUILD_OBJ_DIR ${BUILD_TARGET_DIR}/obj)

if(USE_PLATFORM_UBUNTU)
    add_definitions(-DLIBPARAMS_PARAMS_DIR="${BUILD_SRC_DIR}")
    add_definitions(-DLIBPARAMS_INIT_PARAMS_FILE_NAME="default_params")
    add_definitions(-DLIBPARAMS_TEMP_PARAMS_FILE_NAME="temp_params")
endif()

include(${LIBPARAMS_PATH}/${LIBPARAMS_CMAKE})

list(APPEND APPLICATION_HEADERS ${ROOT_DIR}/Src ${BOARD_DIR}/board)
if(NOT "${NC_APP_SRC_DIR}" STREQUAL "")
    if(EXISTS "${NC_APP_SRC_DIR}" AND IS_DIRECTORY "${NC_APP_SRC_DIR}")
        list(APPEND APPLICATION_HEADERS ${NC_APP_SRC_DIR})
    endif()
endif()
set(APPLICATION_DIR ${BOARD_DIR})

include(${BOARD_TARGET_FILE})
if(NOT CAN_PROTOCOL)
    message(FATAL_ERROR "CAN_PROTOCOL is not specified by ${BOARD_TARGET_FILE}.")
endif()
set(DOCS_ICD_PATH ${ROOT_DIR}/docs/icd_${CAN_PROTOCOL}.md)

# Prebuild
include(${CMAKE_DIR}/params.cmake)

# Hardware version is not supported at the momemnt, let's hardcode it for a while
add_definitions(-DHW_VERSION_MAJOR=2)
add_definitions(-DHW_VERSION_MINOR=1)

set(CMAKE_CXX_STANDARD 20)
# Project
project(node CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode is enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -DDEBUG")
endif()

include(${CMAKE_DIR}/full_firmware_info.cmake)

include(${ROOT_DIR}/Src/peripheral/adc/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/gpio/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/iwdg/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/irq/CMakeLists.txt)

add_subdirectory(${ROOT_DIR}/Src/platform/${APP_PLATFORM})
