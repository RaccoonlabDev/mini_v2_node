# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(NOT LIBPARAMS_PATH)
    message(SEND_ERROR "LIBPARAMS_PATH is unknown.")
elseif(NOT BUILD_SRC_DIR)
    message(SEND_ERROR "BUILD_SRC_DIR is unknown.")
elseif(NOT LIBPARAMS_PARAMS)
    message(SEND_ERROR "LIBPARAMS_PARAMS is unknown.")
elseif(NOT APPLICATION_DIR)
    message(SEND_ERROR "APPLICATION_DIR is unknown.")

endif()

execute_process(
    COMMAND ${LIBPARAMS_PATH}/scripts/generate_params.py --out-dir ${BUILD_SRC_DIR} -f ${LIBPARAMS_PARAMS}
)

execute_process(
    COMMAND ${LIBPARAMS_PATH}/scripts/generate_docs.py ${LIBPARAMS_PARAMS}
    WORKING_DIRECTORY ${APPLICATION_DIR}
)
