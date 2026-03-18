# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT DEFINED CAN_PROTOCOL OR CAN_PROTOCOL STREQUAL "")
    set(CAN_PROTOCOL cyphal)
endif()
add_definitions(-DCONFIG_USE_CYPHAL=1)
add_definitions(-DLIBCPNODE_CUSTOM_HEAP_SIZE=4096)

rl_include_module(cyphal/core)
rl_include_module(application)
rl_include_module(system)
rl_include_module(circuit_status/cyphal)
rl_include_module(feedback/cyphal)
rl_include_module(rcout)
