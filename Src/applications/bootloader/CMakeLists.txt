# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH APPLICATIONS_DIR)
cmake_path(GET APPLICATIONS_DIR PARENT_PATH SRC_DIR)

add_definitions(-DRELEASE_BUILD=1)


include(${SRC_DIR}/modules/bootloader/CMakeLists.txt)
# include(${SRC_DIR}/modules/cyphal/core/CMakeLists.txt)
include(${SRC_DIR}/modules/system/CMakeLists.txt)

list(APPEND APPLICATION_SOURCES
    ${libparamsSrc}
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap/o1heap.c
    ${ROOT_DIR}/Libs/Cyphal/platform_specific/fdcan/cyphal_transport_can.cpp
)

list(APPEND APPLICATION_HEADERS
    ${libparamsHeaders}
    ${ROOT_DIR}/Libs/Cyphal/include
    ${ROOT_DIR}/Libs/Cyphal/Libs/libcanard/libcanard
    ${ROOT_DIR}/build/nunavut_out
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap
)
