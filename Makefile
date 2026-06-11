# Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
.PHONY: require_target all checks code_style tests upload run clean_releases clean distclean build \
	rl_mini_v2_dronecan rl_mini_v2_dronecan_application rl_mini_v2_dronecan_standalone \
	rl_mini_v2_cyphal rl_mini_v2_cyphal_application rl_mini_v2_cyphal_standalone \
	rl_mini_v2_both rl_mini_v2_both_application rl_mini_v2_both_standalone \
	rl_mini_v3_dronecan rl_mini_v3_dronecan_application rl_mini_v3_dronecan_standalone \
	rl_mini_v3_cyphal rl_mini_v3_cyphal_application rl_mini_v3_cyphal_standalone \
	rl_mini_v3_both rl_mini_v3_both_application rl_mini_v3_both_standalone \
	rl_node_v4_dronecan rl_node_v4_dronecan_application rl_node_v4_dronecan_standalone \
	rl_node_v4_cyphal rl_node_v4_cyphal_application rl_node_v4_cyphal_standalone \
	rl_node_v4_both rl_node_v4_both_application rl_node_v4_both_standalone \
	rl_sitl_dronecan rl_sitl_cyphal \
	cyphal cyphal_v2 cyphal_v3 dronecan dronecan_v2 dronecan_v3 v2 v3 sitl_dronecan sitl_cyphal
.DEFAULT_GOAL := require_target

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
NC_BUILD_DIR?=$(ROOT_DIR)/build

# Generic build inputs:
# NC_BOARD - board path under Src/boards, for example: rl/mini_v2
# NC_TARGET - board target file name, for example: dronecan, cyphal, both, bootloader
# NC_IMAGE_KIND - firmware image kind used by CMake and build dir naming
# NC_APP_SRC_DIR - optional application source directory containing boards/, modules/, drivers/
# NC_BUILD_DIR - build output root directory
#              standalone - full firmware image
#              application - bootloader-linked application image
#              bootloader - bootloader image itself
#              sitl - host-native simulation build
NC_BOARD?=$(BOARD)
NC_TARGET?=$(TARGET)
NC_IMAGE_KIND?=$(if $(IMAGE_KIND),$(IMAGE_KIND),standalone)
NC_APP_SRC_DIR?=
BOARD?=
TARGET?=
IMAGE_KIND?=
BUILD_VARIANT?=$(subst /,_,$(NC_BOARD))_$(NC_TARGET)_$(NC_IMAGE_KIND)
EXTRA_CMAKE_ARGS?=

require_target:
ifeq ($(strip $(MAKECMDGOALS)),)
	@echo "Error: target is not specified."
	@echo "Use generic mode: make build NC_BOARD=<vendor/board> NC_TARGET=<target>"
	@echo "Example: make build NC_BOARD=rl/mini_v2 NC_TARGET=dronecan NC_IMAGE_KIND=application"
	@echo "Or use aliases: rl_mini_v2_dronecan, rl_mini_v3_cyphal, ..."
	@exit 2
else
	@:
endif

all: clean_releases rl_mini_v2_dronecan rl_mini_v2_cyphal rl_mini_v3_dronecan rl_mini_v3_cyphal rl_mini_v3_both rl_sitl_dronecan rl_sitl_cyphal

build: checks
ifeq ($(strip $(NC_BOARD)),)
	$(error NC_BOARD is not set. Use: make build NC_BOARD=<vendor/board> NC_TARGET=<target>)
endif
ifeq ($(strip $(NC_TARGET)),)
	$(error NC_TARGET is not set. Use: make build NC_BOARD=<vendor/board> NC_TARGET=<target>)
endif
	@bash "${ROOT_DIR}/scripts/build.sh" \
		--board "${NC_BOARD}" \
		--target "${NC_TARGET}" \
		--image-kind "${NC_IMAGE_KIND}" \
		--app-src-dir "${NC_APP_SRC_DIR}" \
		--build-dir "${NC_BUILD_DIR}" \
		--build-variant "${BUILD_VARIANT}" \
		--cmake-build-type "${CMAKE_BUILD_TYPE}" \
		-- ${EXTRA_CMAKE_ARGS}

#
# Alias naming rules:
# <vendor>_<board>_<target> builds the bootloader application image
#                           (IMAGE_KIND=application) for that target.
# <vendor>_<board>_<target>_standalone builds the standalone app image explicitly.
#
# rl/mini_v2 aliases
#
rl_mini_v2_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_dronecan_application
rl_mini_v2_dronecan_application: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_dronecan_application
rl_mini_v2_dronecan_standalone: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=dronecan IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v2_dronecan_standalone
rl_mini_v2_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=cyphal IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_cyphal_application
rl_mini_v2_cyphal_application: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=cyphal IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_cyphal_application
rl_mini_v2_cyphal_standalone: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=cyphal IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v2_cyphal_standalone
rl_mini_v2_both: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=both IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_both_application
rl_mini_v2_both_application: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=both IMAGE_KIND=application BUILD_VARIANT=rl_mini_v2_both_application
rl_mini_v2_both_standalone: checks
	$(MAKE) build BOARD=rl/mini_v2 TARGET=both IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v2_both_standalone

#
# rl/mini_v3 aliases
#
rl_mini_v3_dronecan: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_dronecan_application
rl_mini_v3_dronecan_application: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_dronecan_application
rl_mini_v3_dronecan_standalone: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=dronecan IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v3_dronecan_standalone
rl_mini_v3_cyphal: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=cyphal IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_cyphal_application
rl_mini_v3_cyphal_application: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=cyphal IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_cyphal_application
rl_mini_v3_cyphal_standalone: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=cyphal IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v3_cyphal_standalone
rl_mini_v3_both: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=both IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_both_application
rl_mini_v3_both_application: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=both IMAGE_KIND=application BUILD_VARIANT=rl_mini_v3_both_application
rl_mini_v3_both_standalone: checks
	$(MAKE) build BOARD=rl/mini_v3 TARGET=both IMAGE_KIND=standalone BUILD_VARIANT=rl_mini_v3_both_standalone

#
# rl/node_v4 aliases
#
rl_node_v4_dronecan: checks
	$(MAKE) build BOARD=rl/node_v4 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_node_v4_dronecan_application
rl_node_v4_dronecan_application: checks
	$(MAKE) build BOARD=rl/node_v4 TARGET=dronecan IMAGE_KIND=application BUILD_VARIANT=rl_node_v4_dronecan_application
rl_node_v4_dronecan_standalone: checks
	$(MAKE) build BOARD=rl/node_v4 TARGET=dronecan IMAGE_KIND=standalone BUILD_VARIANT=rl_node_v4_dronecan_standalone
rl_node_v4_cyphal: checks
	$(error rl/node_v4 Cyphal is not enabled yet. Use rl_node_v4_dronecan_application or rl_node_v4_dronecan_standalone)
rl_node_v4_cyphal_application: checks
	$(error rl/node_v4 Cyphal is not enabled yet. Use rl_node_v4_dronecan_application)
rl_node_v4_cyphal_standalone: checks
	$(error rl/node_v4 Cyphal is not enabled yet. Use rl_node_v4_dronecan_standalone)
rl_node_v4_both: checks
	$(error rl/node_v4 combined Cyphal+DroneCAN is not enabled yet. Use rl_node_v4_dronecan_application)
rl_node_v4_both_application: checks
	$(error rl/node_v4 combined Cyphal+DroneCAN is not enabled yet. Use rl_node_v4_dronecan_application)
rl_node_v4_both_standalone: checks
	$(error rl/node_v4 combined Cyphal+DroneCAN is not enabled yet. Use rl_node_v4_dronecan_standalone)

#
# rl/sitl aliases
#
rl_sitl_dronecan: checks
	$(MAKE) build BOARD=rl/sitl TARGET=dronecan IMAGE_KIND=sitl BUILD_VARIANT=rl_sitl_dronecan_sitl
rl_sitl_cyphal: checks
	$(MAKE) build BOARD=rl/sitl TARGET=cyphal IMAGE_KIND=sitl BUILD_VARIANT=rl_sitl_cyphal_sitl

#
# Optional vendor/local extensions
#
-include ${ROOT_DIR}/Makefile.vendor
-include ${ROOT_DIR}/Makefile.local

#
# Legacy (for compatibility)
# These aliases intentionally build standalone images for backward compatibility, not bootloader-linked application images.
# Don't extend this section for new targets
#
dronecan: rl_mini_v2_dronecan_standalone
dronecan_v2: rl_mini_v2_dronecan_standalone
cyphal_v2: rl_mini_v2_cyphal_standalone
v2: rl_mini_v2_dronecan_standalone
dronecan_v3: rl_mini_v3_dronecan_standalone
cyphal_v3: rl_mini_v3_cyphal_standalone
cyphal: rl_mini_v3_cyphal_standalone
v3: rl_mini_v3_both_standalone
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
	UPLOAD_GOAL=""; \
	for goal in $(MAKECMDGOALS); do \
		if [ "$$goal" != "upload" ]; then UPLOAD_GOAL="$$goal"; break; fi; \
	done; \
	RELEASE_PATTERN="*.bin"; \
	case "$$UPLOAD_GOAL" in \
		rl_mini_v2_*_application|rl_mini_v2_dronecan|rl_mini_v2_cyphal|rl_mini_v2_both) RELEASE_PATTERN="rl_mini_v2_v2_application_*.bin" ;; \
		rl_mini_v2_*_standalone) RELEASE_PATTERN="rl_mini_v2_v2_standalone_*.bin" ;; \
		rl_mini_v3_*_application|rl_mini_v3_dronecan|rl_mini_v3_cyphal|rl_mini_v3_both) RELEASE_PATTERN="rl_mini_v3_v3_application_*.bin" ;; \
		rl_mini_v3_*_standalone) RELEASE_PATTERN="rl_mini_v3_v3_standalone_*.bin" ;; \
		rl_node_v4_*_application|rl_node_v4_dronecan) RELEASE_PATTERN="rl_node_v4_v4_application_*.bin" ;; \
		rl_node_v4_*_standalone) RELEASE_PATTERN="rl_node_v4_v4_standalone_*.bin" ;; \
	esac; \
	LATEST_TARGET=$$(ls -td ${NC_BUILD_DIR}/release/$$RELEASE_PATTERN 2>/dev/null | head -1); \
	if [ -z "$$LATEST_TARGET" ]; then \
		echo "No release binary matches pattern: ${NC_BUILD_DIR}/release/$$RELEASE_PATTERN"; \
		exit 2; \
	fi; \
	FLASH_ADDRESS=0x08000000; \
	case "$$(basename "$$LATEST_TARGET")" in \
		rl_mini_v2_*_application_*) FLASH_ADDRESS=0x08008000 ;; \
		rl_mini_v3_*_application_*) FLASH_ADDRESS=0x08010000 ;; \
		rl_node_v4_*_application_*) FLASH_ADDRESS=0x08020000 ;; \
	esac; \
	echo "Upload goal: $${UPLOAD_GOAL:-latest release}"; \
	echo "Uploading $$LATEST_TARGET"; \
	echo "Flash address: $$FLASH_ADDRESS"; \
	./scripts/flash.sh "$$LATEST_TARGET" "$$FLASH_ADDRESS"

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
	-rm -fR ${NC_BUILD_DIR}/release
clean:
	-rm -fR ${NC_BUILD_DIR}/*/obj
distclean:
	-rm -fR ${NC_BUILD_DIR}/
