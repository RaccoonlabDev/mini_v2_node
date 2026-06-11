# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_minimum_required(VERSION 3.15.3)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi)

set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER  ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_SIZE          ${TOOLCHAIN_PREFIX}-size)

set(COMMON_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections")
set(WARNING_FLAGS "-Wall -Wextra -Wfloat-equal -Werror -Wundef -Wshadow -Wpointer-arith -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default")

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} ${COMMON_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} ${WARNING_FLAGS} -Wno-unused-parameter -Wno-missing-field-initializers -Wno-volatile -fno-exceptions -fno-rtti")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_FLAGS} -lc -lm -lnosys -specs=nano.specs -Wl,--gc-sections")

if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
endif()
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
    USE_HAL_DRIVER
    STM32H753xx
    USE_FLASH_ECC=0U
    USE_SDIO_TRANSCEIVER=0U
    USBD_USER_REGISTER_CALLBACK=0U
    USE_MULTI_CORE_SHARED_CODE=0U
)
