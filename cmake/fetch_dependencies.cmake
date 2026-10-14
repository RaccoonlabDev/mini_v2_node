# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

# cmake [-DNC_<NAME>_VERSION=...] -P cmake/fetch_dependencies.cmake
#
# Resolves every library without building anything, cloning the ones that are not submodules.
# `make deps` runs it, e.g. on the host before a Docker build that has no SSH key.
include(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake)

get_property(names GLOBAL PROPERTY NC_DEPENDENCIES)
foreach(name IN LISTS names)
    nc_use_dependency(${name})
endforeach()
