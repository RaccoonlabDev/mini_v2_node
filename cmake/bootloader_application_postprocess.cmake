# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(DEFINED SELECT_LATEST_APP_DESCRIPTOR_BIN AND SELECT_LATEST_APP_DESCRIPTOR_BIN)
    if(NOT DEFINED INPUT_DIR OR NOT DEFINED PROJECT_NAME OR NOT DEFINED OUT_FILE)
        message(FATAL_ERROR "INPUT_DIR, PROJECT_NAME and OUT_FILE are required")
    endif()

    file(GLOB APP_DESCRIPTOR_BIN_CANDIDATES "${INPUT_DIR}/${PROJECT_NAME}-*.app*.bin")
    if(NOT APP_DESCRIPTOR_BIN_CANDIDATES)
        message(FATAL_ERROR "App descriptor output image not found in ${INPUT_DIR}")
    endif()

    set(LATEST_FILE "")
    set(LATEST_TS -1)
    foreach(CANDIDATE ${APP_DESCRIPTOR_BIN_CANDIDATES})
        file(TIMESTAMP "${CANDIDATE}" TS "%s")
        if(TS GREATER LATEST_TS)
            set(LATEST_TS ${TS})
            set(LATEST_FILE "${CANDIDATE}")
        endif()
    endforeach()

    if("${LATEST_FILE}" STREQUAL "")
        message(FATAL_ERROR "Failed to select latest app descriptor output image")
    endif()

    file(COPY_FILE "${LATEST_FILE}" "${OUT_FILE}" ONLY_IF_DIFFERENT)
    message(STATUS "Selected app descriptor image: ${LATEST_FILE}")
endif()

function(configure_bootloader_application_postprocess executable)
    if(NOT DEFINED IMAGE_KIND OR NOT IMAGE_KIND STREQUAL "application")
        return()
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(POSTPROCESS_STAMP "${BUILD_OBJ_DIR}/${PROJECT_NAME}.postprocess.stamp")
    set(FIRMWARE_POSTPROCESS_TARGET "${executable}_postprocess")

    add_custom_command(
        OUTPUT ${POSTPROCESS_STAMP}
        COMMAND ${Python3_EXECUTABLE}
            ${ROOT_DIR}/scripts/kocherga_image.py
            --assign-version ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
            --assign-mcu-family-id ${MCU_FAMILY_ID}
            --assign-flag-release ${RELEASE_BUILD_FLAG}
            --assign-flag-dirty ${DIRTY_BUILD_FLAG}
            --assign-timestamp ${BUILD_TIMESTAMP_UTC}
            --assign-vcs-revision-id ${GIT_HASH}
            --side-patch ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
            ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
        COMMAND ${CMAKE_COMMAND}
            -DINPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -DPROJECT_NAME=${PROJECT_NAME}
            -DOUT_FILE=${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
            -DSELECT_LATEST_APP_DESCRIPTOR_BIN=1
            -P ${ROOT_DIR}/cmake/bootloader_application_postprocess.cmake
        COMMAND ${CMAKE_COMMAND} -E touch ${POSTPROCESS_STAMP}
        DEPENDS
            ${FIRMWARE_ARTIFACTS_TARGET}
            ${BUILD_OBJ_DIR}/${PROJECT_NAME}.bin
            ${BUILD_OBJ_DIR}/${PROJECT_NAME}.elf
        COMMENT "Applying Kocherga app descriptor CRC/size patch"
        VERBATIM
    )

    add_custom_target(${FIRMWARE_POSTPROCESS_TARGET} ALL DEPENDS ${POSTPROCESS_STAMP})
    add_dependencies(${FIRMWARE_POSTPROCESS_TARGET} ${FIRMWARE_ARTIFACTS_TARGET})
    set(FIRMWARE_FINAL_TARGET "${FIRMWARE_POSTPROCESS_TARGET}" PARENT_SCOPE)
endfunction()
