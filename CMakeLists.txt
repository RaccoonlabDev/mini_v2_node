cmake_minimum_required(VERSION 3.15.3)
project(cyphal_example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(PLATFORM bxcan)
set(libparamsPath ${CMAKE_CURRENT_LIST_DIR}/Libs/libparams)
set(stm32cubeMxProjectPath ${CMAKE_CURRENT_LIST_DIR}/Libs/mini_v2_stm32cubemx_project)

if(USE_DRONECAN)
    include(Libs/libsqcan/CMakeLists.txt)
    set(libsSourceCode
        ${libparamsPath}/libparams/rom.c
        ${libparamsPath}/libparams/storage.c
        ${libparamsPath}/platform_specific/stm32f103/flash_driver.c
        ${dronecanSources}
    )
    set(libsHeaders
        ${libparamsPath}/libparams/
        ${libparamsPath}/platform_specific/stm32f103/
        ${dronecanHeaders}
    )
    set(applicationSourceCode
        Src/dronecan_application/application.cpp
        Src/dronecan_application/params.cpp
    )
    set(applicationHeaders
        Src/dronecan_application
    )
else()
    add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)
    include(Libs/Cyphal/CMakeLists.txt)
    set(libsSourceCode ${CYPHAL_SRC})
    set(libsHeaders
        Libs/Cyphal/Cyphal
        Libs/Cyphal/Libs/libcanard/libcanard
        ${libparamsPath}/libparams
        Libs/Cyphal/Libs/o1heap/o1heap
        build/nunavut_out
    )
    set(applicationSourceCode
        Src/cyphal_application/application.cpp
        Src/cyphal_application/params.cpp
    )
    set(applicationHeaders
        Src/cyphal_application
    )
endif()


set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_CXX_STANDARD 20)

FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Src/*.c)

set(gitRelatedHeaders
    build/src
)

set(stm32CubeMxGeneratedFiles
    ${coreSources}
    ${driversSources}
    ${stm32cubeMxProjectPath}/startup_stm32f103xb.s
)
set(stm32CubeMxHeaders
    ${stm32cubeMxProjectPath}/Core/Inc
    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${stm32cubeMxProjectPath}/Drivers/CMSIS/Include
    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${stm32CubeMxGeneratedFiles}
    ${libsSourceCode}
    ${applicationSourceCode}
)
target_compile_definitions(${EXECUTABLE} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

include_directories(${libsHeaders})

target_include_directories(${EXECUTABLE} PRIVATE
    ${gitRelatedHeaders}
    ${stm32CubeMxHeaders}
    ${applicationHeaders}
)

target_compile_options(${EXECUTABLE} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -fdata-sections
    -ffunction-sections
    -lc -lm -lnosys
    -specs=nano.specs
    -Wall
    --specs=nosys.specs
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${stm32cubeMxProjectPath}/STM32F103T8Ux_FLASH.ld
    -mcpu=cortex-m3
    -mthumb
    --specs=nosys.specs
    -specs=nano.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-size ${EXECUTABLE}
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)
