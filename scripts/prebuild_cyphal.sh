#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"
BUILD_DIR=${REPOSITORY_PATH}/build/src/

${REPOSITORY_PATH}/Libs/libparams/scripts/params_generate_array_from_yaml.py \
    ${BUILD_DIR} \
    c++ \
    params \
    $@

cd $REPOSITORY_PATH/Src/cyphal_application
${REPOSITORY_PATH}/Libs/libparams/scripts/generate_docs.py $@
