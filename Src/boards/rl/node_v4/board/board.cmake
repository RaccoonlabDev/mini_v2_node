# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM stm32h753xx)
set(STM32_CUBEMX_PROJECT_PATH ${ROOT_DIR}/build/stm32cubemx/rl/node_v4)

if(NOT DEFINED IMAGE_KIND OR IMAGE_KIND STREQUAL "")
    set(IMAGE_KIND standalone)
endif()
if(NOT IMAGE_KIND STREQUAL "standalone" AND
   NOT IMAGE_KIND STREQUAL "application" AND
   NOT IMAGE_KIND STREQUAL "bootloader")
    message(FATAL_ERROR "IMAGE_KIND must be standalone, application, or bootloader, got '${IMAGE_KIND}'")
endif()

set(STM32_LINKER_SCRIPT_TEMPLATE_PATH ${CMAKE_CURRENT_LIST_DIR}/STM32H753XX_FLASH.ld.in)
set(BOARD_FLASH_ORIGIN_HEX 0x08000000)
set(BOARD_FLASH_SIZE_KB 2048)
set(BOOTLOADER_RESERVED_KB 128)

if(LIBPARAMS_STORAGE_BACKEND STREQUAL "")
    set(LIBPARAMS_STORAGE_BACKEND internal_flash CACHE STRING "Parameter storage backend: internal_flash or spifram" FORCE)
endif()

if(LIBPARAMS_STORAGE_BACKEND STREQUAL "internal_flash")
    set(PARAMETERS_RESERVED_KB 256)
elseif(LIBPARAMS_STORAGE_BACKEND STREQUAL "spifram")
    set(PARAMETERS_RESERVED_KB 0)
else()
    message(FATAL_ERROR
        "Unsupported LIBPARAMS_STORAGE_BACKEND='${LIBPARAMS_STORAGE_BACKEND}' "
        "for node_v4. Expected: internal_flash or spifram.")
endif()

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
    math(EXPR LINK_FLASH_LENGTH_KB
         "${BOARD_FLASH_SIZE_KB} - ${BOOTLOADER_RESERVED_KB} - ${PARAMETERS_RESERVED_KB}")
endif()
