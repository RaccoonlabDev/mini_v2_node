# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT EXISTS "${NC_LIBCANOPEN_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "libcanopen is not found at NC_LIBCANOPEN_PATH='${NC_LIBCANOPEN_PATH}'.")
endif()

if(NOT TARGET libcanopen)
    add_subdirectory(${NC_LIBCANOPEN_PATH} ${CMAKE_BINARY_DIR}/libcanopen)
endif()

include(${NC_LIBCANOPEN_PATH}/platform_specific/fdcan/config.cmake)

list(APPEND APPLICATION_SOURCES
    ${CANOPEN_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/canopen.cpp
)

list(APPEND APPLICATION_HEADERS
    ${CANOPEN_PLATFORM_HEADERS}
)

set(CONFIG_USE_CANOPEN ON)
