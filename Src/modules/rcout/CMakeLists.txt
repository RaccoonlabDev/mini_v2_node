# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(SETPOINT_MODULE_DIR)
    return()
endif()
set(SETPOINT_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND LIBPARAMS_PARAMS ${SETPOINT_MODULE_DIR}/params.yaml)
list(APPEND APPLICATION_SOURCES ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
list(APPEND APPLICATION_SOURCES ${CMAKE_CURRENT_LIST_DIR}/router.cpp)

if(CAN_PROTOCOL STREQUAL "dronecan" OR CAN_PROTOCOL STREQUAL "both")
    list(APPEND LIBPARAMS_PARAMS ${SETPOINT_MODULE_DIR}/dronecan_frontend/params.yaml)
    list(APPEND APPLICATION_SOURCES ${CMAKE_CURRENT_LIST_DIR}/dronecan_frontend/dronecan_frontend.cpp)
endif()

if(CAN_PROTOCOL STREQUAL "cyphal" OR CAN_PROTOCOL STREQUAL "both")
    list(APPEND LIBPARAMS_PARAMS ${SETPOINT_MODULE_DIR}/cyphal_frontend/params.yaml)
    list(APPEND APPLICATION_SOURCES ${CMAKE_CURRENT_LIST_DIR}/cyphal_frontend/cyphal_frontend.cpp)
endif()

include(${SRC_DIR}/drivers/rcpwm/CMakeLists.txt)
