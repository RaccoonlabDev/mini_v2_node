# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

add_definitions(-DAPP_VERSION_MAJOR=0)
add_definitions(-DAPP_VERSION_MINOR=2)
add_definitions(-DHW_VERSION_MAJOR=0)
add_definitions(-DHW_VERSION_MINOR=0)

include(${ROOT_DIR}/Libs/Dronecan/CMakeLists.txt)

set(applicationSourceCode
    ${libparamsSrc}
    ${DRONECAN_SOURCES}
    ${ROOT_DIR}/Src/dronecan_application/application.cpp
    ${ROOT_DIR}/Src/dronecan_application/params.cpp
)
set(applicationHeaders
    ${libparamsHeaders}
    ${DRONECAN_HEADERS}
    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/dronecan_application
)
