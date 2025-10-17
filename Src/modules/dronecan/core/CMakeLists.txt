# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# 1. libdcnode
add_subdirectory(${ROOT_DIR}/Libs/Dronecan ${CMAKE_BINARY_DIR}/libdcnode)

# 2. libcanver
include(${ROOT_DIR}/Libs/Dronecan/platform_specific/${CAN_PLATFORM}/config.cmake)

list(APPEND APPLICATION_SOURCES
    ${libparamsSrc}
    ${DRONECAN_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dronecan_module.cpp
)

list(APPEND APPLICATION_HEADERS
    ${libparamsHeaders}
    ${DRONECAN_PLATFORM_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}
)
