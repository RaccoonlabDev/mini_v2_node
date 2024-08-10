# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)

set(libparamsPath ${ROOT_DIR}/Libs/libparams)
include(${ROOT_DIR}/Libs/Cyphal/CMakeLists.txt)

list(APPEND APPLICATION_SOURCES
    ${CYPHAL_SRC}
    ${ROOT_DIR}/Libs/Cyphal/Udral/circuit_status.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cyphal_module.cpp
)

list(APPEND APPLICATION_HEADERS
    ${ROOT_DIR}/Libs/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Libs/libcanard/libcanard
    ${ROOT_DIR}/Libs/libparams/libparams
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap
    ${ROOT_DIR}/build/nunavut_out
)

list(APPEND LIBPARAMS_PARAMS
    ${ROOT_DIR}/Libs/Cyphal/Cyphal/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)
