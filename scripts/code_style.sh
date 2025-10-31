#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"

set -e

QUIET=false
if [[ "$1" == "--quiet" ]]; then
    QUIET=true
fi

SOURCE_PATTERNS=(
    "${REPOSITORY_PATH}/Src/modules/*/*.*pp"
    "${REPOSITORY_PATH}/Src/peripheral/*/*.*pp"
    "${REPOSITORY_PATH}/Src/platform/*/*.*pp"
    "${REPOSITORY_PATH}/Src/common/*.*pp"
    "${REPOSITORY_PATH}/Src/drivers/*/*.*pp"
)

# Expand wildcards to get the actual list of files
# because cpplint does not understand shell wildcards (*/*pp) directly
SOURCE_FILES=()
for pattern in "${SOURCE_PATTERNS[@]}"; do
    for file in $pattern; do
        if [[ -f "$file" ]]; then
            SOURCE_FILES+=("$file")
        fi
    done
done

if [[ ${#SOURCE_FILES[@]} -eq 0 ]]; then
    echo "No files found to lint."
    exit 0
fi

if $QUIET; then
    cpplint "${SOURCE_FILES[@]}" > /dev/null || {
        echo "Code style check failed!"
        exit 1
    }
else
    cpplint "${SOURCE_FILES[@]}"
fi
