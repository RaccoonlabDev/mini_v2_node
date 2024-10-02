# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

CRNT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
TESTS_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
REPO_DIR := $(abspath $(dir $(lastword $(TESTS_DIR))))
BUILD_DIR=${REPO_DIR}/build/Tests

$(info CRNT_DIR: ${CRNT_DIR})
$(info TESTS_DIR: ${TESTS_DIR})
$(info REPO_DIR: ${REPO_DIR})
$(info BUILD_DIR: ${BUILD_DIR})

CFLAGS := "-g -O0 -DDEBUG"
CMAKE_FLAGS := -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CC} -DCMAKE_C_FLAGS=${CFLAGS} -DCMAKE_CXX_FLAGS=${CFLAGS} -DCMAKE_BUILD_TYPE=Debug
# cmake /home/user/Documents/work/mini_v2_node/Tests -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=cc -DCMAKE_C_FLAGS="-g -O0 -DDEBUG" -DCMAKE_CXX_FLAGS="-g -O0 -DDEBUG" -DCMAKE_BUILD_TYPE=Debug

all: test_common

test_common: fft algorithms

create_build_dir:
	mkdir -p ${BUILD_DIR}

coverage: create_build_dir
	cd ${BUILD_DIR} && cmake -DCOVERAGE=1 ${CRNT_DIR} && make
	cd ${BUILD_DIR} && ./algorithms
	cd ${BUILD_DIR} && ./ft

	echo "\n1. Algorithms:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/algorithms.dir${REPO_DIR}/common/algorithms.cpp.gcda

	echo "\n2. FFT:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/fft.dir${REPO_DIR}/common/fft.c.gcda

algorithms: create_build_dir
	cd ${BUILD_DIR} && cmake -S ${CRNT_DIR} && make && ./algorithms

fft: create_build_dir
	cd ${BUILD_DIR} && cmake ${CRNT_DIR} ${CMAKE_FLAGS} && make && ./fft
