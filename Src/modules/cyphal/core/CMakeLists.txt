# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(DOWNLOAD_DIR ${ROOT_DIR}/build/downloads)
set(EXTRACT_DIR ${CMAKE_BINARY_DIR}/external)
include(cmake/use_external_github_library.cmake)

#
# 1. libcpnode
#
add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)
use_external_github_library(libcpnode
    source PonomarevDA/libcpnode v0.11.1 # Jan 25, 2026
)

#
# 2. libcanver
#
include(${DOWNLOAD_DIR}/libcpnode/cmake/transport_specific.cmake)


list(APPEND APPLICATION_SOURCES
    ${LIBCPNODE_TRANSPORT_SOURCES}
    ${libparamsSrc}
    ${CMAKE_CURRENT_LIST_DIR}/cyphal_module.cpp
)

list(APPEND APPLICATION_HEADERS
    ${LIBCPNODE_TRANSPORT_HEADERS}
    ${libparamsHeaders}
    ${ROOT_DIR}/build/nunavut_out
)
