# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(SRC_DRIVERS_RCPWM_DIR)
    return()
endif()
set(SRC_DRIVERS_RCPWM_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_SOURCES
)

include(${ROOT_DIR}/Src/peripheral/pwm/CMakeLists.txt)
