/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#include "periphery/gpio/gpio.hpp"

#include "main.h"

void GPIOPeriphery::set(GPIOPin gpio_pin) {
    switch (gpio_pin) {
        case GPIOPin::TERMINATOR1:
            HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin,
                              GPIO_PIN_SET);
            break;

        case GPIOPin::TERMINATOR2:
            HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port,
                              CAN2_TERMINATOR_Pin, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

bool GPIOPeriphery::get(GPIOPin gpio_pin) {
    switch (gpio_pin) {
        case GPIOPin::TERMINATOR1:
            return HAL_GPIO_ReadPin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin);

        case GPIOPin::TERMINATOR2:
            return HAL_GPIO_ReadPin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin);

        default:
            return GPIO_PIN_SET;
    }
}

void GPIOPeriphery::reset() {
    HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin,
                      GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin,
                      GPIO_PIN_RESET);
}

void GPIOPeriphery::reset(GPIOPin gpio_pin) {
    switch (gpio_pin) {
        case GPIOPin::TERMINATOR1:
            HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin,
                              GPIO_PIN_RESET);
            break;
        case GPIOPin::TERMINATOR2:
            HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin,
                              GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}
