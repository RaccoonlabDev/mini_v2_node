#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"
BUILD_DIR=${REPOSITORY_PATH}/build

set -e

mkdir -p ${BUILD_DIR}/tests/as5600
mkdir -p ${BUILD_DIR}/tests/sht3x
cd ${BUILD_DIR}/tests/as5600 && cmake ${REPOSITORY_PATH}/Src/drivers/as5600/tests && make && ./test_as5600
cd ${BUILD_DIR}/tests/sht3x && cmake ${REPOSITORY_PATH}/Src/drivers/sht3x/tests && make && ./test_sht3x
