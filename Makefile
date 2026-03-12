# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
.PHONY: require_target all checks code_style tests upload run clean_releases clean distclean build \
	rl_mini_v2_default rl_mini_v2_dronecan rl_mini_v2_dronecan_application rl_mini_v2_cyphal \
	rl_mini_v3_default rl_mini_v3_dronecan rl_mini_v3_dronecan_application rl_mini_v3_cyphal rl_mini_v3_both \
	rl_sitl_default rl_sitl_dronecan rl_sitl_cyphal \
	cyphal cyphal_v2 cyphal_v3 dronecan dronecan_v2 dronecan_v3 v2 v3 sitl_dronecan sitl_cyphal
.DEFAULT_GOAL := require_target

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build

# Generic build inputs:
# BOARD - board path under Src/boards, for example: rl/mini_v2
# TARGET - board target file name, for example: default, dronecan, cyphal, both
# IMAGE_KIND - firmware image kind, for example: standalone, application
# FIRMWARE_TYPE - standalone/application/bootloader (for build dir naming)
BOARD?=
TARGET?=
IMAGE_KIND?=
FIRMWARE_TYPE?=$(if $(IMAGE_KIND),$(IMAGE_KIND),standalone)
BUILD_VARIANT?=$(subst /,_,$(BOARD))_$(TARGET)_$(FIRMWARE_TYPE)

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

all: clean_releases rl_mini_v2_dronecan rl_mini_v2_cyphal rl_mini_v3_dronecan rl_mini_v3_cyphal rl_mini_v3_both rl_sitl_dronecan rl_sitl_cyphal

build: checks
ifeq ($(strip $(BOARD)),)
	$(error BOARD is not set. Use: make build BOARD=<vendor/board> TARGET=<target>)
endif
ifeq ($(strip $(TARGET)),)
	$(error TARGET is not set. Use: make build BOARD=<vendor/board> TARGET=<target>)
endif
	@OBJ_DIR=${BUILD_DIR}/${BUILD_VARIANT}/obj; \
	mkdir -p $$OBJ_DIR; \
	NEED_CONFIG=0; \
		if [ ! -f $$OBJ_DIR/CMakeCache.txt ]; then \
			NEED_CONFIG=1; \
		elif ! grep -q "^BOARD:STRING=${BOARD}$$" $$OBJ_DIR/CMakeCache.txt; then \
			NEED_CONFIG=1; \
		elif ! grep -q "^BOARD_TARGET:STRING=${TARGET}$$" $$OBJ_DIR/CMakeCache.txt; then \
			NEED_CONFIG=1; \
		elif ! grep -q "^IMAGE_KIND:STRING=${IMAGE_KIND}$$" $$OBJ_DIR/CMakeCache.txt; then \
			NEED_CONFIG=1; \
		fi; \
		if [ $$NEED_CONFIG -eq 1 ]; then \
			echo "Configuring $$OBJ_DIR"; \
			cd $$OBJ_DIR && cmake -DBOARD=${BOARD} -DBOARD_TARGET=${TARGET} -DIMAGE_KIND=${IMAGE_KIND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G "Unix Makefiles" ../../..; \
		else \
			echo "Configuration is up to date: $$OBJ_DIR"; \
		fi; \
		$(MAKE) -C $$OBJ_DIR

#
# rl/mini_v2 aliases
#
rl_mini_v2_default: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=default IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v2_default_standalone
rl_mini_v2_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v2_dronecan_standalone
rl_mini_v2_dronecan_application: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan IMAGE_KIND=application FIRMWARE_TYPE=application BUILD_VARIANT=rl_mini_v2_dronecan_application
rl_mini_v2_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=cyphal IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v2_cyphal_standalone
rl_mini_v2_both: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=both IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v2_both_standalone

#
# rl/mini_v3 aliases
#
rl_mini_v3_default: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=default IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v3_default_standalone
rl_mini_v3_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v3_dronecan_standalone
rl_mini_v3_dronecan_application: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan IMAGE_KIND=application FIRMWARE_TYPE=application BUILD_VARIANT=rl_mini_v3_dronecan_application
rl_mini_v3_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=cyphal IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v3_cyphal_standalone
rl_mini_v3_both: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=both IMAGE_KIND=standalone FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_mini_v3_both_standalone

#
# rl/sitl aliases
#
rl_sitl_default: checks
	$(MAKE) build BOARD=rl/sitl TARGET=default FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_sitl_default_standalone
rl_sitl_dronecan: checks
	$(MAKE) build BOARD=rl/sitl TARGET=dronecan FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_sitl_dronecan_standalone
rl_sitl_cyphal: checks
	$(MAKE) build BOARD=rl/sitl TARGET=cyphal FIRMWARE_TYPE=standalone BUILD_VARIANT=rl_sitl_cyphal_standalone

#
# Legacy (for compatibility)
# Don't extend this section for new targets
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
	LATEST_TARGET=$$(ls -td ${BUILD_DIR}/release/*.bin | head -1) && ./scripts/flash.sh $$LATEST_TARGET

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
