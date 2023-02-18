cmake_minimum_required(VERSION 3.15.3)
project(mini_v2_example CXX C ASM)

set(PLATFORM bxcan)
# include(Libs/Cyphal/CMakeLists.txt)

set(TOOLCHAIN_PREFIX /opt/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

FILE(GLOB coreSources       Core/Src)
FILE(GLOB driversSources    Drivers/STM32F1xx_HAL_Driver/Src)


set(STM32CUBEMX_GENERATED_FILES
    mini_v2/Core/Src/main.cpp
    mini_v2/Core/Src/stm32f1xx_hal_msp.c
    mini_v2/Core/Src/stm32f1xx_hal_timebase_tim.c
    mini_v2/Core/Src/stm32f1xx_it.c
    mini_v2/Core/Src/system_stm32f1xx.c

    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c

    mini_v2/startup_stm32f103xb.s
)

message(STM32CUBEMX_GENERATED_FILES="${STM32CUBEMX_GENERATED_FILES}")

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES})
target_compile_definitions(${EXECUTABLE} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

target_include_directories(${EXECUTABLE} PRIVATE
    mini_v2/Core/Inc
    mini_v2/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    mini_v2/Drivers/CMSIS/Include
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Inc
    mini_v2/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
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
    COMMAND arm-none-eabi-size ${EXECUTABLE})

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)
