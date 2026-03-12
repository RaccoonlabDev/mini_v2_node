# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

if(DEFINED MODULE_APPLICATION_INCLUDED)
    return()
endif()
set(MODULE_APPLICATION_INCLUDED ON)

list(APPEND APPLICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/application.cpp
)

if((USE_PLATFORM_NODE_V2 OR USE_PLATFORM_NODE_V3) AND
   IMAGE_KIND STREQUAL "application")
    list(APPEND APPLICATION_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/app_descriptor.cpp
    )

    set_source_files_properties(
        ${CMAKE_CURRENT_LIST_DIR}/app_descriptor.cpp
        PROPERTIES
        COMPILE_DEFINITIONS
        "BUILD_TIMESTAMP_UTC=${BUILD_TIMESTAMP_UTC};RELEASE_BUILD=${RELEASE_BUILD_FLAG};DIRTY_BUILD=${DIRTY_BUILD_FLAG}"
    )
endif()
