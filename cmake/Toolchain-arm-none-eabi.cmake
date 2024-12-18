# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT DEFINED TARGET_ARCHITECTURE)
    message(SEND_ERROR "TARGET_ARCHITECTURE variable is not specified.")
elseif(NOT DEFINED CPU)
    message(SEND_ERROR "CPU variable is not specified.")
endif()

# System name and processor
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compiler to be used
set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

# GNU Binutils
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

# Compiler and linker options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${TARGET_ARCHITECTURE} -mthumb -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=${TARGET_ARCHITECTURE} -mthumb -fdata-sections -ffunction-sections")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${TARGET_ARCHITECTURE} -mthumb -lc -lm -lnosys -specs=nano.specs -T${ldFile} -Wl,-Map=${PROJECT_NAME}.map,--cref -Wl,--gc-sections")

add_compile_definitions(
    USE_HAL_DRIVER
    ${CPU}
)
