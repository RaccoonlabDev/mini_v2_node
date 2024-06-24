# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
cmake_minimum_required(VERSION 3.15.3)
project(example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${ROOT_DIR}/Src)
set(CMAKE_DIR ${ROOT_DIR}/cmake)
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(BUILD_SRC_DIR ${ROOT_DIR}/build/src)

if(CAN_PROTOCOL STREQUAL "dronecan" OR CAN_PROTOCOL STREQUAL "cyphal")
    set(APPLICATION_DIR ${ROOT_DIR}/Src/${CAN_PROTOCOL}_application)
else()
    message(SEND_ERROR "CAN_PROTOCOL is unknown.")
endif()

# platform
if(USE_PLATFORM_UBUNTU)
    set(LIBPARAMS_PLATFORM ubuntu)
    set(CAN_PLATFORM socketcan)
    set(APP_PLATFORM ubuntu)
else()
    set(stm32cubeMxProjectPath ${ROOT_DIR}/Libs/stm32-cube-project)
    FILE(GLOB ldFile ${stm32cubeMxProjectPath}/*_FLASH.ld)
    get_filename_component(FILE_NAME ${ldFile} NAME_WE)

    if("x${FILE_NAME}" MATCHES "^xSTM32F103.*")
        set(LIBPARAMS_PLATFORM stm32f103)
        set(CAN_PLATFORM bxcan)
        set(APP_PLATFORM stm32f103)
        set(TARGET_ARCHITECTURE cortex-m3)
        set(CPU STM32F103xB)
    elseif("x${FILE_NAME}" MATCHES "^xSTM32G0B1.*")
        set(LIBPARAMS_PLATFORM stm32g0b1)
        set(CAN_PLATFORM fdcan)
        set(APP_PLATFORM stm32f103)
        set(TARGET_ARCHITECTURE cortex-m0plus)
        set(CPU STM32G0B1xx)
    else()
        message("Error: File name does not match any expected pattern.")
    endif()
endif()

include(${LIBPARAMS_PATH}/CMakeLists.txt)
include(${APPLICATION_DIR}/CMakeLists.txt)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-volatile")

# Prebuild
include(${CMAKE_DIR}/params.cmake)
include(${CMAKE_DIR}/git.cmake)

# Hardware version is not supported at the momemnt, let's hardcode it for a while
add_definitions(-DHW_VERSION_MAJOR=2)
add_definitions(-DHW_VERSION_MINOR=1)

set(CMAKE_CXX_STANDARD 20)

include(${ROOT_DIR}/Src/platform/${APP_PLATFORM}/CMakeLists.txt)
set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${APPLICATION_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/module.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${APPLICATION_HEADERS}
    ${ROOT_DIR}/Src/common
    ${PLATFORM_HEADERS}
)

target_compile_options(${EXECUTABLE} PRIVATE
    -Wall
    -Wextra
    -Wfloat-equal
    -Werror
    -Wundef
    -Wshadow
    -Wpointer-arith
    -Wunreachable-code
    -Wstrict-overflow=5
    -Wwrite-strings
    -Wswitch-default
)


if(USE_PLATFORM_UBUNTU)
else()
    target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -D${CPU}
        -DCYPHAL_NUM_OF_CAN_BUSES=1
    )
    target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=${TARGET_ARCHITECTURE}
        -mthumb
        -fdata-sections
        -ffunction-sections
        -lc -lm -lnosys
        -specs=nano.specs
        --specs=nosys.specs
    )

    target_link_options(${EXECUTABLE} PRIVATE
        -T${ldFile}
        -mcpu=${TARGET_ARCHITECTURE}
        -mthumb
        --specs=nosys.specs
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
    )

    # Post-build
    add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE} ${EXECUTABLE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little ${EXECUTABLE} ${PROJECT_NAME}.elf
    )
endif()
