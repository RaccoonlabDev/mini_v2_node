#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPOSITORY_PATH="$(dirname "$SCRIPT_DIR")"

LIBS_DIR="$REPOSITORY_PATH/build/bootloader_v3/libs"
KOCHERGA_REPO="https://github.com/Zubax/kocherga.git"
KOCHERGA_COMMIT="d806ba1"
BOOTLOADER_IOC_REPO="https://github.com/RaccoonLabHardware/v3-software-template.git"
BOOTLOADER_IOC_COMMIT="dc90159"

# Step 1: Create the libs directory if it doesn't exist
if [ ! -d "$LIBS_DIR" ]; then
    echo "Creating directory $LIBS_DIR"
    mkdir -p "$LIBS_DIR"
fi

# Step 2: Clone and checkout kocherga if it doesn't exist
if [ ! -d "$LIBS_DIR/kocherga" ]; then
    echo "Cloning kocherga repository"
    git clone "$KOCHERGA_REPO" "$LIBS_DIR/kocherga"
    cd "$LIBS_DIR/kocherga" || exit
    echo "Checking out commit $KOCHERGA_COMMIT"
    git checkout "$KOCHERGA_COMMIT"
else
    echo "kocherga folder already exists in $LIBS_DIR"
fi

# Step 3: Clone and checkout bootloader-ioc if it doesn't exist
if [ ! -d "$LIBS_DIR/bootloader-ioc" ]; then
    echo "Cloning bootloader-ioc repository"
    git clone "$BOOTLOADER_IOC_REPO" "$LIBS_DIR/bootloader-ioc"
    cd "$LIBS_DIR/bootloader-ioc" || exit
    echo "Checking out commit $BOOTLOADER_IOC_COMMIT"
    git checkout "$BOOTLOADER_IOC_COMMIT"
else
    echo "bootloader-ioc folder already exists in $LIBS_DIR"
fi
