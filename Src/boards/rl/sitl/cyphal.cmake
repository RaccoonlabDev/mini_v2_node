# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

add_definitions(-DCONFIG_USE_CYPHAL=1)
add_definitions(-DLIBCPNODE_CUSTOM_HEAP_SIZE=4096)

include(${ROOT_DIR}/Src/modules/cyphal/core/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/system/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/circuit_status/cyphal/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/feedback/cyphal/CMakeLists.txt)
include(${ROOT_DIR}/Src/modules/rcout/CMakeLists.txt)
