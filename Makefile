# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
.PHONY: require_target all checks code_style tests upload run clean_releases clean distclean build \
	rl_mini_v2_default rl_mini_v2_dronecan rl_mini_v2_cyphal \
	rl_mini_v3_default rl_mini_v3_dronecan rl_mini_v3_cyphal rl_mini_v3_both \
	rl_sitl_default rl_sitl_dronecan rl_sitl_cyphal \
	cyphal cyphal_v2 cyphal_v3 dronecan dronecan_v2 dronecan_v3 v2 v3 sitl_dronecan sitl_cyphal
.DEFAULT_GOAL := require_target

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build

# Generic build inputs:
# BOARD - board path under Src/boards, for example: rl/mini_v2
# TARGET - board target file name, for example: default, dronecan, cyphal, both
BOARD?=
TARGET?=
BUILD_VARIANT?=$(subst /,_,$(BOARD))_$(TARGET)

require_target:
ifeq ($(strip $(MAKECMDGOALS)),)
	@echo "Error: target is not specified."
	@echo "Use generic mode: make build BOARD=<vendor/board> TARGET=<target>"
	@echo "Example: make build BOARD=rl/mini_v2 TARGET=default"
	@echo "Or use aliases: rl_mini_v2_default, rl_mini_v3_cyphal, ..."
	@exit 2
else
	@:
endif

all: clean_releases rl_mini_v2_default rl_mini_v2_cyphal rl_mini_v3_default rl_mini_v3_cyphal rl_mini_v3_both rl_sitl_default rl_sitl_cyphal

build: checks
ifeq ($(strip $(BOARD)),)
	$(error BOARD is not set. Use: make build BOARD=<vendor/board> TARGET=<target>)
endif
ifeq ($(strip $(TARGET)),)
	$(error TARGET is not set. Use: make build BOARD=<vendor/board> TARGET=<target>)
endif
	mkdir -p ${BUILD_DIR}/${BUILD_VARIANT}/obj
	cd ${BUILD_DIR}/${BUILD_VARIANT}/obj && cmake -DBOARD=${BOARD} -DBOARD_TARGET=${TARGET} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../.. && make

#
# rl/mini_v2 aliases
#
rl_mini_v2_default: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=default BUILD_VARIANT=dronecan_v2
rl_mini_v2_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan BUILD_VARIANT=dronecan_v2
rl_mini_v2_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=cyphal BUILD_VARIANT=cyphal_v2
rl_mini_v2_both: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=both BUILD_VARIANT=both_v2

#
# rl/mini_v3 aliases
#
rl_mini_v3_default: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=default BUILD_VARIANT=dronecan_v3
rl_mini_v3_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan BUILD_VARIANT=dronecan_v3
rl_mini_v3_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=cyphal BUILD_VARIANT=cyphal_v3
rl_mini_v3_both: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=both BUILD_VARIANT=both_v3

#
# rl/sitl aliases
#
rl_sitl_default: checks
	$(MAKE) build BOARD=rl/sitl TARGET=default BUILD_VARIANT=dronecan_sitl
rl_sitl_dronecan: checks
	$(MAKE) build BOARD=rl/sitl TARGET=dronecan BUILD_VARIANT=dronecan_sitl
rl_sitl_cyphal: checks
	$(MAKE) build BOARD=rl/sitl TARGET=cyphal BUILD_VARIANT=cyphal_sitl

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
