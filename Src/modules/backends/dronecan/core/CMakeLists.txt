# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# 1. libdcnode
nc_use_dependency(libdcnode)
add_subdirectory(${NC_LIBDCNODE_PATH} ${CMAKE_BINARY_DIR}/libdcnode)

# 2. libcanver
include(${NC_LIBDCNODE_PATH}/platform_specific/${CAN_PLATFORM}/config.cmake)

list(APPEND APPLICATION_SOURCES
    ${LIBPARAMS_PLATFORM_SOURCES}
    ${DRONECAN_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dronecan_module.cpp
)

list(APPEND APPLICATION_HEADERS
    ${LIBPARAMS_PLATFORM_HEADERS}
    ${DRONECAN_PLATFORM_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}
)
