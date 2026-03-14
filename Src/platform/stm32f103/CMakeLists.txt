# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(EXECUTABLE ${PROJECT_NAME})

#
# @todo Later rcpwm_channels.cpp and pwm.cpp should be refactored and moven to Src/boards
#
if(BOARD_TARGET_BOOTLOADER)
    set(PLATFORM_BOARD_MONITOR_SOURCES)
    set(PLATFORM_COMMON_ALGORITHMS_SOURCES)
else()
    set(PLATFORM_BOARD_MONITOR_SOURCES
        ${ROOT_DIR}/Src/drivers/board_monitor/board_monitor.cpp
    )
    set(PLATFORM_COMMON_ALGORITHMS_SOURCES
        ${ROOT_DIR}/Src/common/algorithms.cpp
    )
endif()

add_executable(${EXECUTABLE}
    ${APPLICATION_SOURCES}
    ${PERIPHERAL_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${PLATFORM_COMMON_ALGORITHMS_SOURCES}
    ${ROOT_DIR}/Src/common/logging.cpp
    ${ROOT_DIR}/Src/common/module.cpp

    ${PLATFORM_BOARD_MONITOR_SOURCES}
    ${ROOT_DIR}/Src/platform/stm32/platform_specific.cpp

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

if(CAN_PROTOCOL STREQUAL "dronecan" OR
   CAN_PROTOCOL STREQUAL "both")
    target_link_libraries(${EXECUTABLE} PRIVATE libdcnode::libdcnode)
endif()
if(CAN_PROTOCOL STREQUAL "cyphal" OR CAN_PROTOCOL STREQUAL "both")
    target_link_libraries(${EXECUTABLE} PRIVATE libcpnode::libcpnode)
endif()

# Post-build
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} ${EXECUTABLE}
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little ${EXECUTABLE} ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
)

include(${CMAKE_DIR}/bootloader_application_postprocess.cmake)
configure_bootloader_application_postprocess(${EXECUTABLE})

include(${CMAKE_DIR}/create_binary_with_meaningful_name.cmake)
