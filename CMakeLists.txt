cmake_minimum_required(VERSION 3.15.3)
project(cyphal_example CXX C ASM)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


add_definitions(-DBXCAN_MAX_IFACE_INDEX=0)
set(PLATFORM bxcan)
include(Libs/Cyphal/CMakeLists.txt)

set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

FILE(GLOB coreSources       mini_v2/Core/Src/*)
FILE(GLOB driversSources    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/*.c)


set(STM32CUBEMX_GENERATED_FILES
    ${coreSources}
    ${driversSources}
    ${CYPHAL_SRC}

    Src/simple_app_example/application.cpp
    Src/simple_app_example/params.cpp

    mini_v2/startup_stm32f103xb.s
)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES})
target_compile_definitions(${EXECUTABLE} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

include_directories(Libs/Cyphal/Cyphal)
include_directories(Libs/Cyphal/Libs/libcanard/libcanard)
include_directories(Libs/Cyphal/Libs/libparams/libparams)
include_directories(Libs/Cyphal/Libs/o1heap/o1heap)
include_directories(build/nunavut_out)

target_include_directories(${EXECUTABLE} PRIVATE
    mini_v2/Core/Inc
    mini_v2/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    mini_v2/Drivers/CMSIS/Include
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Inc
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy

    Src/simple_app_example
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
    -T${CMAKE_SOURCE_DIR}/mini_v2/STM32F103T8Ux_FLASH.ld
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
