# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(PERIPHERAL_PWM_CMAKE)
    return()
endif()
set(PERIPHERAL_PWM_CMAKE ${CMAKE_CURRENT_LIST_DIR})

if(NOT APP_PLATFORM)
  message(SEND_ERROR "APP_PLATFORM is not specified or unsupported! Options: stm32f103, stm32g0b1, ubuntu.")
endif()

if(APP_PLATFORM STREQUAL "stm32f103" OR APP_PLATFORM STREQUAL "stm32g0b1")
    list(APPEND PERIPHERAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/pwm_stm32.cpp)
elseif(APP_PLATFORM STREQUAL "ubuntu")
    list(APPEND PERIPHERAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/pwm_ubuntu.cpp)
else()
    message(SEND_ERROR "APP_PLATFORM is unknown.")
endif()
