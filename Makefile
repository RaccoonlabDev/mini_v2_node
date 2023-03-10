# Copyright (c) 2022 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

build: clean
	mkdir -p build
	cd build && cmake .. && make

upload:
	./scripts/flash.sh build/cyphal_example.bin

generage_dsdl:
	mkdir -p build/nunavut_out
	./Libs/Cyphal/scripts/nnvg_generate_c_headers.sh

clean:
	-rm -fR build/
