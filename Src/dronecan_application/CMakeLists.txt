# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

add_definitions(-DFLASH_DRIVER_STORAGE_FILE="${CMAKE_CURRENT_LIST_DIR}/default_params.yml")

include(${ROOT_DIR}/Libs/Dronecan/CMakeLists.txt)

set(APPLICATION_SOURCES
    ${libparamsSrc}
    ${DRONECAN_SOURCES}
    ${ROOT_DIR}/Src/dronecan_application/application.cpp
    ${ROOT_DIR}/Src/dronecan_application/modules/circuit_status/CircuitStatusModule.cpp
    ${ROOT_DIR}/Src/dronecan_application/modules/pwm/PWMModule.cpp
    ${ROOT_DIR}/Src/dronecan_application/logger.cpp

)
set(APPLICATION_HEADERS
    ${libparamsHeaders}
    ${DRONECAN_HEADERS}
    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/dronecan_application
)

set(LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/modules/pwm/params.yaml
)
