# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(SYSTEM_MODULE_DIR)
    return()
endif()
set(SYSTEM_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}
)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

if(USE_PLATFORM_NODE_V3)
    set(PARAMS_SYSTEM_CAN_TERMINATOR params_can_terminator_enabled.yaml)
else()
    set(PARAMS_SYSTEM_CAN_TERMINATOR params_can_terminator_reserved.yaml)
endif()

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params_common.yaml
    ${CMAKE_CURRENT_LIST_DIR}/${PARAMS_SYSTEM_CAN_TERMINATOR}
    ${CMAKE_CURRENT_LIST_DIR}/params_protocol_${CAN_PROTOCOL}.yaml
)

include(${ROOT_DIR}/Src/peripheral/led/CMakeLists.txt)
