# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
cmake_minimum_required(VERSION 3.15.3)
project(example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
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

if(USE_PLATFORM_UBUNTU)
else()
    set(TOOLCHAIN_PREFIX arm-none-eabi-)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_CXX_STANDARD 20)
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c)
    FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                                ${stm32cubeMxProjectPath}/Core/Startup/*.s
    )
    set(stm32CubeMxGeneratedFiles
        ${coreSources}
        ${driversSources}
        ${startupFile}
    )
    set(stm32CubeMxHeaders
        ${stm32cubeMxProjectPath}/Core/Inc
        ${stm32cubeMxProjectPath}/Drivers/CMSIS/Include

        ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
        ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc
        ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy

        ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32G0xx/Include
        ${stm32cubeMxProjectPath}/Drivers/STM32G0xx_HAL_Driver/Inc
        ${stm32cubeMxProjectPath}/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
    )
endif()

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${stm32CubeMxGeneratedFiles}
    ${applicationSourceCode}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/adc.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/pwm.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/iwdg.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/led.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/temperature_sensor.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/platform_specific.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/i2c.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/sht3x.cpp

)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${stm32CubeMxHeaders}
    ${applicationHeaders}
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-volatile")

execute_process(
    COMMAND ${LIBPARAMS_PATH}/scripts/generate_params.py --out-dir ${BUILD_SRC_DIR} -f ${LIBPARAMS_PARAMS}
)

execute_process(
    COMMAND ${LIBPARAMS_PATH}/scripts/generate_docs.py ${LIBPARAMS_PARAMS}
    WORKING_DIRECTORY ${APPLICATION_DIR}
)

execute_process(
    COMMAND git rev-parse --short=16 HEAD
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_VARIABLE GIT_HASH_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(GIT_HASH "0x${GIT_HASH_SHORT}")
add_definitions(-DGIT_HASH=${GIT_HASH})

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
        -Wall
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

    add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE}
    )

    add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        COMMAND arm-none-eabi-objcopy -I binary -O elf32-little ${EXECUTABLE} ${PROJECT_NAME}.elf
    )
endif()
