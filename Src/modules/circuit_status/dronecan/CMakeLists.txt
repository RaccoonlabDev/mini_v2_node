# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/circuit_status.cpp
)

list(APPEND LIBPARAMS_PARAMS
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
)

include(${SRC_DIR}/peripheral/adc/CMakeLists.txt)
