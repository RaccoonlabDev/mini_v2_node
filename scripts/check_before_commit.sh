#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"
BUILD_DIR=${REPOSITORY_PATH}/build/

set -e

cd $REPOSITORY_PATH
targets=(cyphal sitl_cyphal dronecan sitl_dronecan)
for target in ${targets[@]}; do
    make -s distclean
    echo $target
    make -s ${target}
done
