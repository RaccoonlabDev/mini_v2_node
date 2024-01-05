/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022-2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "periphery/pwm/pwm.hpp"

int8_t PwmPeriphery::init(PwmPin pwm_pin) {
    (void)pwm_pin;
    return 0;
}

void PwmPeriphery::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    (void)pwm_pin;
    (void)duration_us;

}

uint32_t PwmPeriphery::get_duration(PwmPin pwm_pin) {
    (void)pwm_pin;
    return 0;
}
