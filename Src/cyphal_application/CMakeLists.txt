# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(APPLICATION_DIR ${CMAKE_CURRENT_LIST_DIR})

include(${APPLICATION_DIR}/modules/cyphal/CMakeLists.txt)
include(${APPLICATION_DIR}/modules/circuit_status/CMakeLists.txt)
include(${APPLICATION_DIR}/modules/feedback/CMakeLists.txt)
include(${APPLICATION_DIR}/modules/setpoint/CMakeLists.txt)
