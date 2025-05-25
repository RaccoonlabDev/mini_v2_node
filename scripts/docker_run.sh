#!/bin/bash
# Copyright (C) 2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
set -e

IMAGE_NAME=mini-node-docker

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR=${ROOT_DIR}/build/

HOST_UID=$(id -u)
HOST_GID=$(id -g)

docker build -t $IMAGE_NAME "$ROOT_DIR"

docker run --rm \
           -v "$BUILD_DIR":/workspace/build \
           -v "$ROOT_DIR":/workspace \
           -w /workspace \
           -e HOST_UID=$HOST_UID \
           -e HOST_GID=$HOST_GID \
           $IMAGE_NAME \
           bash -c "GIT_DISCOVERY_ACROSS_FILESYSTEM=1 make dronecan && chown -R \$HOST_UID:\$HOST_GID /workspace/build"