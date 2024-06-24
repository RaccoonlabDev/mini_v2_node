# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(PLATFORM_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iwdg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/led.cpp
    ${CMAKE_CURRENT_LIST_DIR}/temperature_sensor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific.cpp
)

set(PLATFORM_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}
)
