# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# create_binary_with_meaningful_name.cmake

# Ensure the required variables are defined
if(NOT DEFINED BUILD_OBJ_DIR OR NOT DEFINED PROJECT_NAME)
    message(FATAL_ERROR "BUILD_OBJ_DIR and PROJECT_NAME must be defined")
endif()

set(SOURCE_BIN "${BUILD_OBJ_DIR}/example.bin")
# message(FATAL_ERROR "${SOURCE_BIN}")

# Define the source and destination paths
set(DESTINATION_DIR "${BUILD_ROOT_DIR}/release")
set(DESTINATION_BIN "${DESTINATION_DIR}/node_${PLATFORM_NAME}_${CAN_PROTOCOL}_${LATEST_COMMIT_DATE}_v${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}_${GIT_HASH_SHORT_8_DIGITS}.bin")

# Create the release directory if it doesn't exist
file(MAKE_DIRECTORY "${DESTINATION_DIR}")

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin ${DESTINATION_BIN}
)