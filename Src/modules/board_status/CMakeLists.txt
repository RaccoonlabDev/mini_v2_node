# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# Every frontend includes the logic module, so a target with two frontends must get it once.
include_guard(GLOBAL)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/board_status.cpp
)

include(${SRC_DIR}/peripheral/adc/CMakeLists.txt)
