# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

add_definitions(-DFLASH_DRIVER_STORAGE_FILE="${CMAKE_CURRENT_LIST_DIR}/default_params.yml")
add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)

set(libparamsPath ${ROOT_DIR}/Libs/libparams)
include(${ROOT_DIR}/Libs/Cyphal/CMakeLists.txt)

set(APPLICATION_SOURCES
    ${CYPHAL_SRC}
    ${ROOT_DIR}/Libs/Cyphal/Udral/circuit_status.cpp

    ${ROOT_DIR}/Src/cyphal_application/modules/circuit_status/circuit_status.cpp
    ${ROOT_DIR}/Src/cyphal_application/modules/cyphal/cyphal_module.cpp
    ${ROOT_DIR}/Src/cyphal_application/modules/setpoint/setpoint.cpp
    ${ROOT_DIR}/Src/cyphal_application/modules/feedback/feedback.cpp
    ${ROOT_DIR}/Src/cyphal_application/modules/modules.cpp
)
set(APPLICATION_HEADERS
    ${ROOT_DIR}/Libs/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Libs/libcanard/libcanard
    ${ROOT_DIR}/Libs/libparams/libparams
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap
    ${ROOT_DIR}/build/nunavut_out

    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/cyphal_application
    ${ROOT_DIR}/Src/cyphal_application/modules
)

set(LIBPARAMS_PARAMS
    ${ROOT_DIR}/Libs/Cyphal/Cyphal/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/modules/cyphal/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/modules/circuit_status/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/modules/setpoint/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/modules/feedback/params.yaml
)
