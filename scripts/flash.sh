#!/bin/bash
# https://gist.github.com/PonomarevDA/829961851abeca5c51154a0882a625fa
# This software is distributed under the terms of the MIT License.
# Copyright (c) 2023-2026 Dmitry Ponomarev.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

SCRIPT_NAME="flash.sh"
VERSION="v1.1.0"

BINARY_FILE=$1
FLASH_ADDRESS=${2:-0x08000000}

RED='\033[0;31m'
NC='\033[0m'
if [ -z "$BINARY_FILE" ]; then
    echo -e "${RED}${SCRIPT_NAME} ${VERSION} error on line $LINENO: expected an argument!${NC}"
    exit
elif [ ! -f "$BINARY_FILE" ]; then
    echo -e "${RED}${SCRIPT_NAME} ${VERSION} error on line $LINENO: specified file $BINARY_FILE is not exist!${NC}"
    exit
elif [[ ! $BINARY_FILE == *.bin ]]; then
    echo -e "${RED}${SCRIPT_NAME} ${VERSION} error on line $LINENO: speficied file must have .bin extension!${NC}"
    exit
elif [[ ! $FLASH_ADDRESS =~ ^0x[0-9a-fA-F]+$ ]]; then
    echo -e "${RED}${SCRIPT_NAME} ${VERSION} error on line $LINENO: flash address must be a hex value, got $FLASH_ADDRESS!${NC}"
    exit
fi

echo "${SCRIPT_NAME} ${VERSION}: binary $BINARY_FILE"
echo "${SCRIPT_NAME} ${VERSION}: address $FLASH_ADDRESS"

for attempt in {1..25}; do
    echo ""
    echo "${SCRIPT_NAME} ${VERSION}: attempt $attempt. Trying to load the firmware..."
    echo -en "\007"

    output=$(st-info --probe)

    is_programmer_not_found=$(echo "$output" | grep "Found 0 stlink programmers")
    if [ ! -z "$is_programmer_not_found" ]; then
        echo "${SCRIPT_NAME} ${VERSION}: stlink programmer not found. Please connect it to your USB port!"
        sleep 2
        continue
    fi

    is_device_not_found=$(echo "$output" | grep "unknown device")
    if [ ! -z "$is_device_not_found" ]; then
        echo "${SCRIPT_NAME} ${VERSION}: STM32 not found. Please connect it to the stlink programmer!"
        sleep 2
        continue
    fi

    # stm32f103 has a wrong flash size. It says 64 KBytes, but it is actually 128 KBytes!
    # Medium-density for old versions of st-link, MD for the newer versions of st-link
    is_stm32f103=$(echo "$output" | grep "F1xx Medium-density\|STM32F1xx_MD")
    if [ ! -z "$is_stm32f103" ]; then
        EXPLICIT_FLASH_SIZE="--flash=0x00020000"
    fi

    output=$(st-flash $EXPLICIT_FLASH_SIZE --connect-under-reset --reset write $BINARY_FILE $FLASH_ADDRESS 2>&1 | tee /dev/tty)
    write_status=${PIPESTATUS[0]}
    is_write_verified=$(echo "$output" | grep -E "Flash written and verified|pages written")
    if [ "$write_status" -ne 0 ] || [ -z "$is_write_verified" ]; then
        sleep 2
    else
        echo "${SCRIPT_NAME} ${VERSION}: done"
        for attempt in {1..15}; do
            sleep 0.1
            echo -en "\007"
        done
        exit
    fi
done

for attempt in {1..15}; do
    sleep 0.1
    echo -en "\007"
done
