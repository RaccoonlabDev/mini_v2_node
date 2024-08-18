# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

include(${ROOT_DIR}/Libs/Dronecan/CMakeLists.txt)

list(APPEND APPLICATION_SOURCES
    ${libparamsSrc}
    ${DRONECAN_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dronecan_module.cpp
    ${CMAKE_CURRENT_LIST_DIR}/logger.cpp
)

list(APPEND APPLICATION_HEADERS
    ${libparamsHeaders}
    ${DRONECAN_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}
)
