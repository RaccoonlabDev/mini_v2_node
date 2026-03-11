# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(PERIPHERAL_CAN_CMAKE)
    return()
endif()
set(PERIPHERAL_CAN_CMAKE ${CMAKE_CURRENT_LIST_DIR})

if(NOT APP_PLATFORM)
  message(SEND_ERROR "APP_PLATFORM is not specified or unsupported! Options: stm32g0b1.")
endif()

if(APP_PLATFORM STREQUAL "stm32g0b1")
    list(APPEND PERIPHERAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/can_stm32g0.cpp)
else()
    message(SEND_ERROR "CAN peripheral is implemented for stm32g0b1 only.")
endif()

