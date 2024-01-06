# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

set(libparamsPath ${ROOT_DIR}/Libs/libparams)
add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)
include(${ROOT_DIR}/Libs/Cyphal/CMakeLists.txt)
set(libsSourceCode ${CYPHAL_SRC})
set(libsHeaders
    ${ROOT_DIR}/Libs/Cyphal/Cyphal
    ${ROOT_DIR}/Libs/Cyphal/Libs/libcanard/libcanard
    ${libparamsPath}/libparams
    ${ROOT_DIR}/Libs/Cyphal/Libs/o1heap/o1heap
    ${ROOT_DIR}/build/nunavut_out
)

set(applicationSourceCode
    ${ROOT_DIR}/Src/cyphal_application/setpoint/setpoint.cpp
    ${ROOT_DIR}/Src/cyphal_application/feedback/feedback.cpp
    ${ROOT_DIR}/Src/cyphal_application/application.cpp

    ${ROOT_DIR}/build/src/params.cpp
    ${ROOT_DIR}/build/src/string_params.cpp
)
set(applicationHeaders
    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/cyphal_application
    ${ROOT_DIR}/Libs/Cyphal
)

set(cyphalRegisters ${ROOT_DIR}/Libs/Cyphal/Cyphal/params.yaml)
list(APPEND cyphalRegisters
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/setpoint/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/feedback/params.yaml
)

add_definitions(-DFLASH_DRIVER_STORAGE_FILE="${ROOT_DIR}/Src/cyphal_application/default_params.yml")
