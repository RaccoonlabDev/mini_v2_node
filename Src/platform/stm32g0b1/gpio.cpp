/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#include "peripheral/gpio/gpio.hpp"

#include "main.h"

void GPIOPeriphery::set(GPIOPin gpio_pin) {
#ifdef CAN1_TERMINATOR_GPIO_Port
    switch (gpio_pin) {
        case GPIOPin::CAN_TERMINATOR1:
            HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin, GPIO_PIN_SET);
            break;

        case GPIOPin::CAN_TERMINATOR2:
            HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin, GPIO_PIN_SET);
            break;
        default:
            break;
    }
#else
    (void)gpio_pin;
#endif
}

bool GPIOPeriphery::get(GPIOPin gpio_pin) {
#ifdef CAN1_TERMINATOR_GPIO_Port
    switch (gpio_pin) {
        case GPIOPin::CAN_TERMINATOR1:
            return HAL_GPIO_ReadPin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin);

        case GPIOPin::CAN_TERMINATOR2:
            return HAL_GPIO_ReadPin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin);

        default:
            return GPIO_PIN_SET;
    }
#else
    (void)gpio_pin;
    return GPIO_PIN_RESET;
#endif
}

void GPIOPeriphery::reset() {
#ifdef CAN1_TERMINATOR_GPIO_Port
    HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin, GPIO_PIN_RESET);
#endif
}

void GPIOPeriphery::reset(GPIOPin gpio_pin) {
#ifdef CAN1_TERMINATOR_GPIO_Port
    switch (gpio_pin) {
        case GPIOPin::CAN_TERMINATOR1:
            HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin, GPIO_PIN_RESET);
            break;
        case GPIOPin::CAN_TERMINATOR2:
            HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
#else
    (void)gpio_pin;
#endif
}
