# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# The libraries node-core builds on, each with its default source. An application builds another
# version with -DNC_<NAME>_VERSION=<tag or commit> or -DNC_<NAME>_PATH=<checkout>, see
# README.md, Libraries. Keep one declaration per line: scripts/check_dependency_pins.py reads them.
include(${CMAKE_CURRENT_LIST_DIR}/dependency.cmake)

nc_declare_dependency(libparams PonomarevDA/libparams SUBMODULE Libs/libparams)
nc_declare_dependency(libdcnode PonomarevDA/dronecan_application SUBMODULE Libs/Dronecan)
nc_declare_dependency(libcpnode PonomarevDA/libcpnode VERSION v0.13.2)
