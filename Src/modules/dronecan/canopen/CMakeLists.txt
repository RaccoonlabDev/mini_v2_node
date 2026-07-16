# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT EXISTS "${NC_LIBCANOPEN_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "libcanopen is not found at NC_LIBCANOPEN_PATH='${NC_LIBCANOPEN_PATH}'.")
endif()

if(NOT TARGET libcanopen)
    add_subdirectory(${NC_LIBCANOPEN_PATH} ${CMAKE_BINARY_DIR}/libcanopen)
endif()

list(APPEND APPLICATION_SOURCES
    ${ROOT_DIR}/Src/peripheral/can/canopen_stm32h7.cpp
    ${CMAKE_CURRENT_LIST_DIR}/canopen.cpp
)

set(CONFIG_USE_CANOPEN ON)
