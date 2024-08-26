# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build
BUILD_OBJ_DIR:=$(BUILD_DIR)/obj

# Cyphal
NUNAVUT_OUT_DIR:=$(BUILD_DIR)/nunavut_out
IS_DSDL_GENERATED:=$(shell if [ -d ${NUNAVUT_OUT_DIR} ]; then find ${NUNAVUT_OUT_DIR} -mindepth 1 | wc -l; else echo 0; fi)
generate_dsdl:
	mkdir -p ${NUNAVUT_OUT_DIR}
	@if [ "$(IS_DSDL_GENERATED)" -eq 0 ]; then \
		echo "[INFO] Cyphal DSDL: generate to ${NUNAVUT_OUT_DIR}"; \
		./Libs/Cyphal/scripts/nnvg_generate_c_headers.sh ${NUNAVUT_OUT_DIR} > /dev/null 2>&1; \
	else \
		echo "[INFO] Cyphal DSDL: already generated. Skip."; \
	fi
cyphal: cyphal_v2
cyphal_v2: checks generate_dsdl clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_NODE_V2=ON -G "Unix Makefiles" ../.. && make
sitl_cyphal: checks generate_dsdl clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_UBUNTU=ON -G "Unix Makefiles" ../.. && make
cyphal_v3: checks generate_dsdl clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_NODE_V3=ON -G "Unix Makefiles" ../.. && make

# Dronecan:
dronecan: dronecan_v2
dronecan_v2: checks clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_NODE_V2=ON -G "Unix Makefiles" ../.. && make
sitl_dronecan: checks clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_UBUNTU=ON -G "Unix Makefiles" ../.. && make
dronecan_v3: checks clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_NODE_V3=ON -G "Unix Makefiles" ../.. && make

# Cyphal & DroneCAN
v3: checks generate_dsdl clean
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DCAN_PROTOCOL=both -DUSE_PLATFORM_NODE_V3=ON -G "Unix Makefiles" ../.. && make

# Common:
checks:
	@python scripts/prebuild_check.py || (echo "Requirements verification failed. Stopping build." && exit 1)

upload:
	./scripts/tools/stm32/flash.sh ${BUILD_OBJ_DIR}/example.bin
run:
	./scripts/tools/can/vcan.sh slcan0
	./build/obj/example.out
clean:
	-rm -fR ${BUILD_OBJ_DIR}/
distclean:
	-rm -fR ${BUILD_DIR}/
