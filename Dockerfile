# Copyright (C) 2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
# Dockerfile
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install system packages
RUN apt-get update && \
    apt-get install -y \
    git \
    make \
    cmake \
    curl \
    wget \
    python3 \
    python3-pip \
    build-essential \
    ca-certificates \
    gcc-arm-none-eabi \
    python-is-python3 \
    && rm -rf /var/lib/apt/lists/*

# Install Python packages
COPY requirements.txt requirements.txt
RUN pip install -r requirements.txt

WORKDIR /workspace

RUN git config --global --add safe.directory /workspace

COPY .git .git
COPY Libs Libs
COPY Src Src
COPY cmake cmake
COPY scripts scripts
COPY CMakeLists.txt CMakeLists.txt
COPY Makefile Makefile
COPY requirements.txt requirements.txt
