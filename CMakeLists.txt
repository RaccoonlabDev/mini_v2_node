cmake_minimum_required(VERSION 3.15.3)
project(example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# libparams
set(LIBPARAMS_PATH ${ROOT_DIR}/Libs/libparams)
set(LIBPARAMS_PLATFORM stm32f103)
include(${ROOT_DIR}/Libs/libparams/CMakeLists.txt)

if(USE_DRONECAN)
    include(${ROOT_DIR}/Src/dronecan_application/CMakeLists.txt)
else()
    include(${ROOT_DIR}/Src/cyphal_application/CMakeLists.txt)
endif()

if(USE_PLATFORM_UBUNTU)
    message(SEND_ERROR "USE_PLATFORM_UBUNTU is not supported yet!")
else()
    set(TOOLCHAIN_PREFIX arm-none-eabi-)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_CXX_STANDARD 20)
    set(stm32cubeMxProjectPath ${ROOT_DIR}/Libs/stm32-cube-project)
    FILE(GLOB coreSources       ${stm32cubeMxProjectPath}/Core/Src/*)
    FILE(GLOB driversSources    ${stm32cubeMxProjectPath}/Drivers/STM32F1xx_HAL_Driver/Src/*.c*)
    set(gitRelatedHeaders
        ${ROOT_DIR}/build/src
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
endif()

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE}
    ${libparams}
    ${stm32CubeMxGeneratedFiles}
    ${libsSourceCode}
    ${applicationSourceCode}
)

if(USE_PLATFORM_UBUNTU)
    message(SEND_ERROR "USE_PLATFORM_UBUNTU is not supported yet!")
else()
    target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
    )
endif()

include_directories(${libsHeaders})

target_include_directories(${EXECUTABLE} PRIVATE
    ${gitRelatedHeaders}
    ${stm32CubeMxHeaders}
    ${applicationHeaders}
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-volatile")

if(USE_PLATFORM_UBUNTU)
    message(SEND_ERROR "USE_PLATFORM_UBUNTU is not supported yet!")
else()
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
endif()

if(NOT USE_DRONECAN)
    execute_process(
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/scripts/prebuild_cyphal.sh ${cyphalRegisters}
    )
endif()

if(USE_PLATFORM_UBUNTU)
    message(SEND_ERROR "USE_PLATFORM_UBUNTU is not supported yet!")
else()
    add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE}
    )

    add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        COMMAND arm-none-eabi-objcopy -I binary -O elf32-little ${EXECUTABLE} ${PROJECT_NAME}.elf
    )
endif()
