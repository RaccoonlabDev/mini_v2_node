# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include the toolchain file
include(${CMAKE_DIR}/Toolchain-arm-none-eabi.cmake)

# Collect source files
set(stm32cubeMxProjectPath ${ROOT_DIR}/Libs/stm32-cube-project)
FILE(GLOB ldFile ${stm32cubeMxProjectPath}/*_FLASH.ld)
FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/*/*/*.c)
FILE(GLOB startupFile       ${stm32cubeMxProjectPath}/*.s
                            ${stm32cubeMxProjectPath}/Core/Startup/*.s
)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${APPLICATION_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/module.cpp

    ${CMAKE_CURRENT_LIST_DIR}/adc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iwdg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/led.cpp
    ${CMAKE_CURRENT_LIST_DIR}/temperature_sensor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific.cpp

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
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
)