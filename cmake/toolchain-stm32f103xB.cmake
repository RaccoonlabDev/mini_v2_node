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
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ==================================
#  Compiler Paths
# ==================================
set(TOOLCHAIN_PREFIX arm-none-eabi)

set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER  ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_SIZE          ${TOOLCHAIN_PREFIX}-size)

# ==================================
#  Compilation and Linker Flags
# ==================================
set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -fdata-sections -ffunction-sections")
set(WARNING_FLAGS "-Wall -Wextra -Wfloat-equal -Werror -Wundef -Wshadow -Wpointer-arith -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default")  # these could be also useful: -Wconversion -Wsign-conversion

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} ${COMMON_FLAGS} ${WARNING_FLAGS}")  # these could be also useful: -Wmissing-prototypes -Wstrict-prototypes
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} ${WARNING_FLAGS} -Wno-volatile -fno-exceptions -fno-rtti")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=cortex-m3 -mthumb -lc -lm -lnosys -specs=nano.specs -Wl,--gc-sections")

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
# set(CMAKE_CXX_EXTENSIONS OFF)  # todo

# ==================================
#  Preprocessor Definitions
# ==================================
add_compile_definitions(
    USE_HAL_DRIVER
    STM32F103xB
)
