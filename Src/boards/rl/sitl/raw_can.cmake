# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Example target: the DroneCAN SITL node that also sends the board status as raw CAN
# frames. CI checks it with Tests/test_raw_can_board_status.py; it is not a release target.
include(${CMAKE_CURRENT_LIST_DIR}/dronecan.cmake)
rl_include_module(backends/raw_can/board_status)
