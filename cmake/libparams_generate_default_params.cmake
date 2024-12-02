# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

execute_process(
    COMMAND ${LIBPARAMS_PATH}/scripts/generate_default_params.py
    --out-dir ${BUILD_SRC_DIR}
    -f ${BUILD_SRC_DIR}/params.cpp
    --out-file-name default_params
    RESULT_VARIABLE ret
)
if(NOT ret EQUAL 0)
    message( FATAL_ERROR "Default Params Generator has been failed. Abort.")
endif()
