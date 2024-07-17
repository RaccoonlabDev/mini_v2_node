# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(TARGET_ARCHITECTURE cortex-m0plus)
set(CPU STM32G0B1xx)
set(stm32cubeMxProjectPath ${ROOT_DIR}/Libs/mini-v3-ioc)
FILE(GLOB ldFile ${stm32cubeMxProjectPath}/*_FLASH.ld)
FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c)
FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                            ${stm32cubeMxProjectPath}/Core/Startup/*.s
)
include(${CMAKE_CURRENT_LIST_DIR}/Toolchain-arm-none-eabi.cmake)
