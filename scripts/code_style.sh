#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"

set -e

cpplint ${REPOSITORY_PATH}/Src/modules/*/*pp \
        ${REPOSITORY_PATH}/Src/peripheral/*/*pp Src/platform/*/*pp