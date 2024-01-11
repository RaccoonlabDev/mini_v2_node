cmake_minimum_required(VERSION 3.15.3)
project(example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Pathes
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(BUILD_SRC_DIR ${ROOT_DIR}/build/src)
if(USE_DRONECAN)
    set(APPLICATION_DIR ${ROOT_DIR}/Src/dronecan_application)
else()
    set(APPLICATION_DIR ${ROOT_DIR}/Src/cyphal_application)
endif()

# platform
if(USE_PLATFORM_UBUNTU)
    set(LIBPARAMS_PLATFORM ubuntu)
    set(CAN_PLATFORM socketcan)
    set(APP_PLATFORM ubuntu)
else()
    set(LIBPARAMS_PLATFORM stm32f103)
    set(CAN_PLATFORM bxcan)
    set(APP_PLATFORM stm32f103)
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
    set(stm32cubeMxProjectPath ${ROOT_DIR}/Libs/stm32-cube-project)
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Src/*.c*)
    set(stm32CubeMxGeneratedFiles
        ${coreSources}
        ${driversSources}
        ${stm32cubeMxProjectPath}/startup_stm32f103xb.s
    )
    set(stm32CubeMxHeaders
        ${stm32cubeMxProjectPath}/Core/Inc
        ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
        ${stm32cubeMxProjectPath}/Drivers/CMSIS/Include
        ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc
        ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    )
endif()

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${stm32CubeMxGeneratedFiles}
    ${applicationSourceCode}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/pwm.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/led.cpp
    ${ROOT_DIR}/Src/platform/${APP_PLATFORM}/platform_specific.cpp
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
        -DSTM32F103xB
    )
    target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -fdata-sections
        -ffunction-sections
        -lc -lm -lnosys
        -specs=nano.specs
        -Wall
        --specs=nosys.specs
    )

    target_link_options(${EXECUTABLE} PRIVATE
        -T${stm32cubeMxProjectPath}/STM32F103T8Ux_FLASH.ld
        -mcpu=cortex-m3
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
