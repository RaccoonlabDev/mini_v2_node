# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT DEFINED CAN_PROTOCOL OR CAN_PROTOCOL STREQUAL "")
    set(CAN_PROTOCOL dronecan)
endif()
add_definitions(-DCONFIG_USE_DRONECAN=1)

rl_include_module(application)
rl_include_module(system)

rl_include_module(dronecan/core)

if(NOT NODE_V4_DIAG_MINIMAL_MODULES)
    rl_include_module(dronecan/arming)
    rl_include_module(circuit_status/dronecan)
endif()
