#!/bin/bash
# Brief: Get sniffer symlink
# Usage: source get_sniffer_symlinks.sh
# Output environment variables:
# - DEV_PATH
# - DEV_PATH_SYMLINK

EXPECTED_VID=0483
EXPECTED_PID=374b
EXPECTED_DEV_PATH="/dev/ttyACM*"
EXPECTED_SYMLINK_PATH="/dev/serial/by-id/"

for dev_path in $EXPECTED_DEV_PATH; do
    [ -e "$dev_path" ] || continue
    check_vid_and_pid=$(udevadm info $dev_path |
                        grep -E "(ID_MODEL_ID=$EXPECTED_PID|ID_VENDOR_ID=$EXPECTED_VID)" -wc)
    if [ "$check_vid_and_pid" == 2 ]
    then
        DEV_PATH=$dev_path
        DEV_PATH_SYMLINK=$(find -L $EXPECTED_SYMLINK_PATH -samefile $DEV_PATH)
    fi
done
