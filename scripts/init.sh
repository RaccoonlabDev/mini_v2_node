#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"
INIT_SCRIPT_PATH=$SCRIPT_DIR/tools/cyphal/init.sh
DSDL_DIR="$REPOSITORY_PATH/Libs/Cyphal/Libs/public_regulated_data_types"

export REG_DATA_TYPE_PATH="$DSDL_DIR/uavcan $DSDL_DIR/reg"
export YAKUT_COMPILE_OUTPUT="$REPOSITORY_PATH/build/compile_output"

if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
    $INIT_SCRIPT_PATH
else
    source $INIT_SCRIPT_PATH
fi
