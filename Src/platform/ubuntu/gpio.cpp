/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#include "periphery/gpio/gpio.hpp"

#include <vector>
#include "main.h"

static std::vector<bool> gpio((int)GPIOPin::GPIO_AMOUNT, false);

void GPIOPeriphery::set(GPIOPin gpio_pin) {
    gpio[(int)gpio_pin] = true;
}
bool GPIOPeriphery::get(GPIOPin gpio_pin) {
    return gpio[(int)gpio_pin];
}
void GPIOPeriphery::reset() {
    for (auto gpio_val : gpio) {
        gpio_val = false;
    }
}
void GPIOPeriphery::reset(GPIOPin gpio_pin) {
    gpio[(int)gpio_pin] = false;
}
