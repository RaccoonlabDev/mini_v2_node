#!/bin/bash

if [ "${BASH_SOURCE[0]}" -ef "$0" ]
then
    echo "ERROR: you should source this script, not execute it!"
    exit
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$SCRIPT_DIR/.."
REG_DATA_TYPE_PATH="$REPOSITORY_PATH/Libs/Cyphal/Libs/public_regulated_data_types/uavcan $REPOSITORY_PATH/Libs/Cyphal/Libs/public_regulated_data_types/reg/"

res=$(ifconfig | grep slcan0)
if [ -z "$res" ]; then
    $SCRIPT_DIR/create_slcan_from_serial.sh $CYPHAL_DEV_PATH_SYMLINK
fi

export YAKUT_COMPILE_OUTPUT="$REPOSITORY_PATH/build_dsdl"
export YAKUT_PATH="$YAKUT_COMPILE_OUTPUT"
export UAVCAN__CAN__IFACE='socketcan:slcan0'
export UAVCAN__CAN__MTU=8
export UAVCAN__NODE__ID=127

if [ -d $YAKUT_COMPILE_OUTPUT ] && [ "$(ls -A $YAKUT_COMPILE_OUTPUT)" ]; then
    :
else
    yakut compile $REG_DATA_TYPE_PATH -O $YAKUT_COMPILE_OUTPUT
fi
