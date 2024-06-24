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

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${APPLICATION_SOURCES}
    ${BUILD_SRC_DIR}/params.cpp
    ${ROOT_DIR}/Src/common/algorithms.cpp
    ${ROOT_DIR}/Src/common/module.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${BUILD_SRC_DIR}
    ${APPLICATION_HEADERS}
    ${ROOT_DIR}/Src/common
    ${PLATFORM_HEADERS}
)

target_compile_options(${EXECUTABLE} PRIVATE
    -Wall
    -Wextra
    -Wfloat-equal
    -Werror
    -Wundef
    -Wshadow
    -Wpointer-arith
    -Wunreachable-code
    -Wstrict-overflow=5
    -Wwrite-strings
    -Wswitch-default
)

# Post-build
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EXECUTABLE} ${BUILD_OBJ_DIR}/${EXECUTABLE}
)
