# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Pathes
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH PLATFORM_DIR)
cmake_path(GET PLATFORM_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${APPLICATION_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/application.cpp
    ${ROOT_DIR}/Src/common/module.cpp

    ${ROOT_DIR}/Src/periphery/adc/circuit_periphery.cpp
    ${ROOT_DIR}/Src/platform/stm32/pwm/pwm.cpp
    ${PLATFORM_DIR}/stm32f103/adc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gpio.cpp
    ${PLATFORM_DIR}/stm32g0b1/pwm.cpp
    ${PLATFORM_DIR}/stm32g0b1/spi.cpp
    ${PLATFORM_DIR}/stm32f103/iwdg.cpp
    ${PLATFORM_DIR}/stm32f103/led.cpp
    ${PLATFORM_DIR}/stm32f103/temperature_sensor.cpp
    ${PLATFORM_DIR}/stm32f103/platform_specific.cpp

    ${coreSources}
    ${driversSources}
    ${startupFile}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${APPLICATION_HEADERS}
    ${ROOT_DIR}/Src/common

    ${stm32cubeMxProjectPath}/Core/Inc
    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Include

    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy

    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32G0xx/Include
    ${stm32cubeMxProjectPath}/Drivers/STM32G0xx_HAL_Driver/Inc
    ${stm32cubeMxProjectPath}/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
)

# Post-build
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} ${EXECUTABLE}
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
)
