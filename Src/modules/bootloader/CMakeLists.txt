# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(BOOTLOADER_MODULE_DIR)
    return()
endif()
set(BOOTLOADER_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}
    ${ROOT_DIR}/build/bootloader_v3/libs/kocherga/kocherga/
)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bootloader_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/my_can_driver.cpp
)

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)

include(${ROOT_DIR}/Src/peripheral/led/CMakeLists.txt)
