# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(EXECUTABLE ${PROJECT_NAME})

add_executable(${EXECUTABLE}
    ${APPLICATION_SOURCES}
    ${PERIPHERAL_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/logging.cpp
    ${ROOT_DIR}/Src/common/module.cpp

    ${ROOT_DIR}/Src/drivers/board_monitor/board_monitor.cpp
    ${ROOT_DIR}/Src/platform/stm32/platform_specific.cpp

    ${coreSources}
    ${driversSources}
    ${middlewareSources}
    ${usbDeviceSources}
    ${startupFile}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${APPLICATION_HEADERS}
    ${ROOT_DIR}/Src/common
    ${CMAKE_CURRENT_LIST_DIR}
    ${stm32cubeMxProjectPath}/Core/Inc
    ${stm32cubeMxProjectPath}/USB_DEVICE/App
    ${stm32cubeMxProjectPath}/USB_DEVICE/Target
    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Include

    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
    ${stm32cubeMxProjectPath}/Drivers/STM32H7xx_HAL_Driver/Inc
    ${stm32cubeMxProjectPath}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
    ${stm32cubeMxProjectPath}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${stm32cubeMxProjectPath}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
)

if(CAN_PROTOCOL STREQUAL "dronecan" OR CAN_PROTOCOL STREQUAL "both")
    target_link_libraries(${EXECUTABLE} PRIVATE libdcnode::libdcnode)
endif()
if(CAN_PROTOCOL STREQUAL "cyphal" OR CAN_PROTOCOL STREQUAL "both")
    target_link_libraries(${EXECUTABLE} PRIVATE libcpnode::libcpnode)
endif()

set(FIRMWARE_ARTIFACTS_TARGET "${EXECUTABLE}_artifacts")
set(FIRMWARE_FINAL_TARGET "${FIRMWARE_ARTIFACTS_TARGET}")

add_custom_command(
    OUTPUT
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little $<TARGET_FILE:${EXECUTABLE}> ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
    DEPENDS ${EXECUTABLE}
    COMMENT "Generating firmware artifacts"
    VERBATIM
)

add_custom_target(${FIRMWARE_ARTIFACTS_TARGET} ALL
    DEPENDS
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.hex
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
        ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
)

include(${CMAKE_DIR}/bootloader_application_postprocess.cmake)
configure_bootloader_application_postprocess(${EXECUTABLE})

include(${CMAKE_DIR}/create_binary_with_meaningful_name.cmake)
