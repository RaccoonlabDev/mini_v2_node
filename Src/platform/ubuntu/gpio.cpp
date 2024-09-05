/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#include "periphery/gpio/gpio.hpp"

#include "main.h"

bool gpio[(int)GPIOPin::GPIO_AMOUNT];

void GPIOPeriphery::set(GPIOPin gpio_pin) {
    gpio[(int)gpio_pin] = 1;
}
bool GPIOPeriphery::get(GPIOPin gpio_pin) {
    return gpio[(int)gpio_pin];
}
void GPIOPeriphery::reset() {
    for (uint8_t i = 0; i < (int)GPIOPin::GPIO_AMOUNT; i++ ) {
        gpio[i] = 0;
    }
}
void GPIOPeriphery::reset(GPIOPin gpio_pin) {
    gpio[(int)gpio_pin] = 0;
}
