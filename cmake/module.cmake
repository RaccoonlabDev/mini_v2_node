# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

set(NC_APP_SRC_DIR "" CACHE PATH "Optional application source directory containing boards/, modules/, drivers/")
set(APP_ROOT_DIR "" CACHE PATH "Deprecated. Optional application root directory")
set(APP_MODULES_DIR "" CACHE PATH "Deprecated. Optional directory with application-specific modules")
set(APP_DRIVERS_DIR "" CACHE PATH "Deprecated. Optional directory with application-specific drivers")
set(APP_BOARDS_DIR "" CACHE PATH "Deprecated. Optional directory with application-specific boards")

if(NC_APP_SRC_DIR STREQUAL "" AND NOT APP_ROOT_DIR STREQUAL "")
    set(NC_APP_SRC_DIR "${APP_ROOT_DIR}/src")
endif()

if(APP_ROOT_DIR STREQUAL "" AND NOT NC_APP_SRC_DIR STREQUAL "")
    get_filename_component(APP_ROOT_DIR "${NC_APP_SRC_DIR}" DIRECTORY)
endif()

if(APP_MODULES_DIR STREQUAL "" AND NOT NC_APP_SRC_DIR STREQUAL "")
    set(APP_MODULES_DIR "${NC_APP_SRC_DIR}/modules")
endif()
if(APP_DRIVERS_DIR STREQUAL "" AND NOT NC_APP_SRC_DIR STREQUAL "")
    set(APP_DRIVERS_DIR "${NC_APP_SRC_DIR}/drivers")
endif()
if(APP_BOARDS_DIR STREQUAL "" AND NOT NC_APP_SRC_DIR STREQUAL "")
    set(APP_BOARDS_DIR "${NC_APP_SRC_DIR}/boards")
endif()

macro(rl_include_module module_path)
    set(_rl_module_found OFF)

    if(NOT APP_MODULES_DIR STREQUAL "")
        set(app_module_file "${APP_MODULES_DIR}/${module_path}/CMakeLists.txt")
        if(EXISTS "${app_module_file}")
            include("${app_module_file}")
            set(_rl_module_found ON)
        endif()
    endif()

    if(NOT _rl_module_found)
        set(core_module_file "${ROOT_DIR}/Src/modules/${module_path}/CMakeLists.txt")
        if(EXISTS "${core_module_file}")
            include("${core_module_file}")
            set(_rl_module_found ON)
        endif()
    endif()

    if(NOT _rl_module_found)
        message(FATAL_ERROR "Module '${module_path}' is not found.")
    endif()
endmacro()

macro(rl_include_driver driver_path)
    set(_rl_driver_found OFF)

    if(NOT APP_DRIVERS_DIR STREQUAL "")
        set(app_driver_file "${APP_DRIVERS_DIR}/${driver_path}/CMakeLists.txt")
        if(EXISTS "${app_driver_file}")
            include("${app_driver_file}")
            set(_rl_driver_found ON)
        endif()
    endif()

    if(NOT _rl_driver_found)
        set(core_driver_file "${ROOT_DIR}/Src/drivers/${driver_path}/CMakeLists.txt")
        if(EXISTS "${core_driver_file}")
            include("${core_driver_file}")
            set(_rl_driver_found ON)
        endif()
    endif()

    if(NOT _rl_driver_found)
        message(FATAL_ERROR "Driver '${driver_path}' is not found.")
    endif()
endmacro()
