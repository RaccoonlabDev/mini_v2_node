# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM stm32g0b1)
set(STM32_CUBEMX_PROJECT_PATH ${ROOT_DIR}/Libs/mini-v3-ioc)

if(NOT DEFINED IMAGE_KIND OR IMAGE_KIND STREQUAL "")
    set(IMAGE_KIND standalone)
endif()
if(NOT IMAGE_KIND STREQUAL "standalone" AND
   NOT IMAGE_KIND STREQUAL "application" AND
   NOT IMAGE_KIND STREQUAL "bootloader")
    message(FATAL_ERROR "IMAGE_KIND must be standalone, application, or bootloader, got '${IMAGE_KIND}'")
endif()

set(STM32_LINKER_SCRIPT_TEMPLATE_PATH ${CMAKE_CURRENT_LIST_DIR}/STM32G0B1CEUX_FLASH.ld.in)
set(BOARD_FLASH_ORIGIN_HEX 0x08000000)
set(BOARD_FLASH_SIZE_KB 512)
set(BOOTLOADER_RESERVED_KB 64)
list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rcpwm_channels.cpp
)
if(IMAGE_KIND STREQUAL "standalone" OR IMAGE_KIND STREQUAL "bootloader")
    set(LINK_FLASH_ORIGIN_HEX ${BOARD_FLASH_ORIGIN_HEX})
    if(IMAGE_KIND STREQUAL "bootloader")
        set(LINK_FLASH_LENGTH_KB ${BOOTLOADER_RESERVED_KB})
    else()
        set(LINK_FLASH_LENGTH_KB ${BOARD_FLASH_SIZE_KB})
    endif()
else()
    math(EXPR APP_FLASH_ORIGIN_DEC "0x08000000 + ${BOOTLOADER_RESERVED_KB} * 1024")
    set(LINK_FLASH_ORIGIN_HEX ${APP_FLASH_ORIGIN_DEC})
    math(EXPR LINK_FLASH_LENGTH_KB "${BOARD_FLASH_SIZE_KB} - ${BOOTLOADER_RESERVED_KB}")
endif()
