# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
.PHONY: require_target all checks code_style tests upload run clean_releases clean distclean \
	rl_mini_v2_default rl_mini_v2_dronecan rl_mini_v2_cyphal \
	rl_mini_v3_default rl_mini_v3_dronecan rl_mini_v3_cyphal rl_mini_v3_both \
	rl_sitl_default rl_sitl_dronecan rl_sitl_cyphal \
	cyphal cyphal_v2 cyphal_v3 dronecan dronecan_v2 dronecan_v3 v2 v3 sitl_dronecan sitl_cyphal
.DEFAULT_GOAL := require_target

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build


require_target:
ifeq ($(strip $(MAKECMDGOALS)),)
	@echo "Error: target is not specified."
	@echo "Use: make <target>"
	@echo "Targets: all rl_mini_v2_default rl_mini_v2_dronecan rl_mini_v2_cyphal rl_mini_v3_default rl_mini_v3_dronecan rl_mini_v3_cyphal rl_mini_v3_both rl_sitl_default rl_sitl_dronecan rl_sitl_cyphal code_style tests clean clean_releases distclean"
	@exit 2
else
	@:
endif

all: clean_releases rl_mini_v2_default rl_mini_v2_cyphal rl_mini_v3_default rl_mini_v3_cyphal rl_mini_v3_both rl_sitl_default rl_sitl_cyphal

#
# rl/mini_v2
#
rl_mini_v2_default: rl_mini_v2_dronecan
rl_mini_v2_dronecan: checks
	mkdir -p ${BUILD_DIR}/dronecan_v2/obj
	cd ${BUILD_DIR}/dronecan_v2/obj && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_NODE_V2=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make
rl_mini_v2_cyphal: checks
	mkdir -p ${BUILD_DIR}/cyphal_v2/obj
	cd ${BUILD_DIR}/cyphal_v2/obj && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_NODE_V2=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make
rl_mini_v2_both: checks
	mkdir -p ${BUILD_DIR}/both_v2/obj
	cd ${BUILD_DIR}/both_v2/obj && cmake -DCAN_PROTOCOL=both -DUSE_PLATFORM_NODE_V2=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make

#
# rl/mini_v3
#
rl_mini_v3_default: rl_mini_v3_dronecan
rl_mini_v3_dronecan: checks
	mkdir -p ${BUILD_DIR}/dronecan_v3/obj
	cd ${BUILD_DIR}/dronecan_v3/obj && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_NODE_V3=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make
rl_mini_v3_cyphal: checks
	mkdir -p ${BUILD_DIR}/cyphal_v3/obj
	cd ${BUILD_DIR}/cyphal_v3/obj && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_NODE_V3=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make
rl_mini_v3_both: checks
	mkdir -p ${BUILD_DIR}/both_v3/obj
	cd ${BUILD_DIR}/both_v3/obj && cmake -DCAN_PROTOCOL=both -DUSE_PLATFORM_NODE_V3=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make

#
# rl/sitl
#
rl_sitl_default: rl_sitl_dronecan
rl_sitl_dronecan: checks
	mkdir -p ${BUILD_DIR}/dronecan_sitl/obj
# Prevents bug when last modified dir is not sitl so script looks into build/release in run
	touch ${BUILD_DIR}/dronecan_sitl
	cd ${BUILD_DIR}/dronecan_sitl/obj && cmake -DCAN_PROTOCOL=dronecan -DUSE_PLATFORM_UBUNTU=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make
rl_sitl_cyphal: checks
	mkdir -p ${BUILD_DIR}/cyphal_sitl/obj
	cd ${BUILD_DIR}/cyphal_sitl/obj && cmake -DCAN_PROTOCOL=cyphal -DUSE_PLATFORM_UBUNTU=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make

#
# Legacy (for compatibility)
#
dronecan: rl_mini_v2_default
dronecan_v2: rl_mini_v2_dronecan
cyphal_v2: rl_mini_v2_cyphal
v2: rl_mini_v2_default
dronecan_v3: rl_mini_v3_dronecan
cyphal_v3: rl_mini_v3_cyphal
cyphal: rl_mini_v3_cyphal
v3: rl_mini_v3_both
sitl_cyphal: rl_sitl_cyphal
sitl_dronecan: rl_sitl_dronecan

# Common:
checks:
	@python scripts/prebuild_check.py || (echo "Requirements verification failed. Stopping build." && exit 1)

code_style:
	${ROOT_DIR}/scripts/code_style.sh

tests:
	${ROOT_DIR}/scripts/tests.sh

upload:
	LATEST_TARGET=$$(ls -td ${BUILD_DIR}/release/*.bin | head -1) && ./scripts/tools/stm32/flash.sh $$LATEST_TARGET

SOCKETCAN_URL:=https://gist.githubusercontent.com/PonomarevDA/6ecc8fc340e4c50619c1e5dfcedc37b2/raw/2db6d1626a9ada543602ff0a52b48fecb94e6e07/socketcan.sh
SOCKETCAN_EXECUTABLE:=build/tools/socketcan-v1.0.2.sh
download_socketcan:
	@test -x "${SOCKETCAN_EXECUTABLE}" || { \
		mkdir -p build/tools; \
		curl -fsSL "${SOCKETCAN_URL}" -o "${SOCKETCAN_EXECUTABLE}"; \
		chmod +x "${SOCKETCAN_EXECUTABLE}"; \
	}
create_slcan0: download_socketcan
	${SOCKETCAN_EXECUTABLE} --virtual-can --interface slcan0
remove_slcan0: download_socketcan
	${SOCKETCAN_EXECUTABLE} --remove --interface slcan0

run: create_slcan0
	LATEST_TARGET=$$(ls -td build/*/obj/node | head -1 | head -1) && $$LATEST_TARGET
clean_releases:
	-rm -fR ${BUILD_DIR}/release
clean:
	-rm -fR ${BUILD_DIR}/*/obj
distclean:
	-rm -fR ${BUILD_DIR}/
