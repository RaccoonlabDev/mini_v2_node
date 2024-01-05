cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SRC_DIR)
cmake_path(GET SRC_DIR PARENT_PATH ROOT_DIR)

set(PLATFORM bxcan)
include(${ROOT_DIR}/Libs/libsqcan/CMakeLists.txt)
set(libsSourceCode
    ${libparamsSrc}
    ${dronecanSources}
)
set(libsHeaders
    ${libparamsHeaders}
    ${dronecanHeaders}
)

set(applicationSourceCode
    ${ROOT_DIR}/Src/periphery/pwm/pwm.cpp
    ${ROOT_DIR}/Src/periphery/led/led.cpp

    ${ROOT_DIR}/Src/dronecan_application/application.cpp
    ${ROOT_DIR}/Src/dronecan_application/params.cpp
)
set(applicationHeaders
    ${ROOT_DIR}/Src
    ${ROOT_DIR}/Src/dronecan_application
)
