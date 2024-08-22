# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.


list(APPEND APPLICATION_HEADERS
    ${libparamsHeaders}
    ${DRONECAN_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}
)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/arming.cpp
)

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)

message(STATUS "ARM:" ${LIBPARAMS_PARAMS})
