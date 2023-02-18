#!/bin/bash
SCRIPTS_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_DIR="$(dirname "$SCRIPTS_DIR")"

for attempt in {1..25}; do
    echo ""
    echo "Attempt $attempt. Trying to load the firmware..."
    output=$(st-flash --reset write $REPO_DIR/$1 0x8000000 | tee /dev/tty)

    compare_res=$(echo "$output" | grep "pages written")
    if [ -z "$compare_res" ]; then
        sleep 2
    else
        echo Done
        exit
    fi
done