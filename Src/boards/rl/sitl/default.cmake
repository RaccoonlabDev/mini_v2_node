# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(BOARD_PLATFORM ubuntu)
set(CAN_PROTOCOL dronecan)
if(BOARD_CONFIG_ONLY)
    return()
endif()
include(${CMAKE_CURRENT_LIST_DIR}/dronecan.cmake)
