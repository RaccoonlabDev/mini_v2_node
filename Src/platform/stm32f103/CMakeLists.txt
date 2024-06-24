# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include the toolchain file
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Toolchain-arm-none-eabi.cmake)

# Collect source files
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

set(PLATFORM_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iwdg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/led.cpp
    ${CMAKE_CURRENT_LIST_DIR}/temperature_sensor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific.cpp

    ${stm32CubeMxGeneratedFiles}
)

set(PLATFORM_HEADERS
    ${stm32CubeMxHeaders}
)
