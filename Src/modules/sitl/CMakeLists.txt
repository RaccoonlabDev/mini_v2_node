# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Include guard
if(SITL_MODULE_DIR)
    return()
endif()
set(SITL_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND APPLICATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}
)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)
