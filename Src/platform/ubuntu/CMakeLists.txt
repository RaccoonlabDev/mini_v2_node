# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

include(${ROOT_DIR}/Src/peripheral/adc/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/gpio/CMakeLists.txt)
include(${ROOT_DIR}/Src/peripheral/iwdg/CMakeLists.txt)

# Generate temp parameters for SITL
include(${CMAKE_DIR}/libparams_generate_default_params.cmake)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${APPLICATION_SOURCES}
    ${PERIPHERAL_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/logging.cpp
    ${ROOT_DIR}/Src/common/application.cpp
    ${ROOT_DIR}/Src/common/module.cpp

    ${ROOT_DIR}/Src/peripheral/adc/circuit_periphery.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific.cpp
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${APPLICATION_HEADERS}
    ${ROOT_DIR}/Src/common

    ${CMAKE_CURRENT_LIST_DIR}
)

# Post-build
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EXECUTABLE} ${BUILD_OBJ_DIR}/${EXECUTABLE}
)
