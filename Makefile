# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

BUILD_DIR:=build
BUILD_OBJ_DIR:=build/obj
IS_SUBMODULE_INITIALIZED := $(shell find Libs/libparams -mindepth 1 | wc -l)

# Cyphal
generate_dsdl:
	mkdir -p ${BUILD_DIR}/nunavut_out
	./Libs/Cyphal/scripts/nnvg_generate_c_headers.sh
cyphal: check_submodules clean autogenerate_git_related_headers
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake ../.. && make
sitl_cyphal: check_submodules clean autogenerate_git_related_headers
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DUSE_PLATFORM_UBUNTU=1 ../.. && make

# Dronecan:
dronecan: check_submodules clean autogenerate_git_related_headers
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DUSE_DRONECAN=1 ../.. && make
sitl_dronecan: check_submodules clean autogenerate_git_related_headers
	mkdir -p ${BUILD_OBJ_DIR}
	cd ${BUILD_OBJ_DIR} && cmake -DUSE_DRONECAN=1 -DUSE_PLATFORM_UBUNTU=1 ../.. && make

# Common:
check_submodules:
	@if [ "$(IS_SUBMODULE_INITIALIZED)" -eq 0 ]; then \
		echo "Error: submodules are empty. Please type: 'git submodule update --init --recursive'"; \
		exit 1; \
	else \
		echo "Directory is fine"; \
	fi
upload:
	./scripts/tools/stm32/flash.sh ${BUILD_OBJ_DIR}/example.bin
run:
	./scripts/tools/can/vcan.sh slcan0
	./build/obj/example.out
autogenerate_git_related_headers:
	mkdir -p ${BUILD_DIR}/src
	./scripts/tools/stm32/generate_software_version.sh ${BUILD_DIR}/src
clean:
	-rm -fR ${BUILD_OBJ_DIR}/
distclean:
	-rm -fR ${BUILD_DIR}/
