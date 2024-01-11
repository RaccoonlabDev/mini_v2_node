# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

add_definitions(-DFLASH_DRIVER_STORAGE_FILE="${CMAKE_CURRENT_LIST_DIR}/default_params.yml")
add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)

set(libparamsPath ${ROOT_DIR}/Libs/libparams)
include(${ROOT_DIR}/Libs/Cyphal/CMakeLists.txt)

set(applicationSourceCode
    ${CYPHAL_SRC}
    ${ROOT_DIR}/Src/cyphal_application/setpoint/setpoint.cpp
    ${ROOT_DIR}/Src/cyphal_application/feedback/feedback.cpp
    ${ROOT_DIR}/Src/cyphal_application/application.cpp
)
set(applicationHeaders
    ${ROOT_DIR}/Libs/Cyphal/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Libs/libcanard/libcanard
    ${ROOT_DIR}/Libs/libparams/libparams
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap
    ${ROOT_DIR}/build/nunavut_out
    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/cyphal_application
    ${ROOT_DIR}/Libs/Cyphal
)

set(LIBPARAMS_PARAMS
    ${ROOT_DIR}/Libs/Cyphal/Cyphal/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/setpoint/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/feedback/params.yaml
)
