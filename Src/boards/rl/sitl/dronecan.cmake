# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT DEFINED CAN_PROTOCOL OR CAN_PROTOCOL STREQUAL "")
    set(CAN_PROTOCOL dronecan)
endif()
add_definitions(-DCONFIG_USE_DRONECAN=1)

rl_include_module(dronecan/core)
rl_include_module(application)
rl_include_module(system)
rl_include_module(dronecan/arming)
rl_include_module(circuit_status/dronecan)
rl_include_module(feedback/dronecan)
rl_include_module(rcout)
rl_include_module(sitl)

if(USE_PLATFORM_NODE_V3 OR USE_PLATFORM_UBUNTU)
    rl_include_module(imu)
endif()
