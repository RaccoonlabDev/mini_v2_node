# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#
# 1. libcpnode
#
add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)
nc_use_dependency(libcpnode)
add_subdirectory(${NC_LIBCPNODE_PATH} ${CMAKE_BINARY_DIR}/libcpnode)

#
# 2. libcanver
#
include(${NC_LIBCPNODE_PATH}/cmake/transport_specific.cmake)


list(APPEND APPLICATION_SOURCES
    ${LIBCPNODE_TRANSPORT_SOURCES}
    ${LIBPARAMS_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/cyphal_module.cpp
)

list(APPEND APPLICATION_HEADERS
    ${LIBCPNODE_TRANSPORT_HEADERS}
    ${LIBPARAMS_PLATFORM_HEADERS}
    ${ROOT_DIR}/build/nunavut_out
)
