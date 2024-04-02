// Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
#define SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_

#include <stdint.h>
#include "periphery/temperature_sensor/temperature_sensor.hpp"
#include "periphery/adc/adc.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class CircuitPeriphery{
public:
    static int8_t init(){
        return AdcPeriphery::init();
    }

    static uint16_t temperature() {
        uint16_t temp = AdcPeriphery::get(AdcChannel::ADC_TEMPERATURE);
        return stm32TemperatureParse(temp);
    }

    static float current() {
        constexpr float ADC_CURRENT_MULTIPLIER = 10.0f / 4095.0f;  // 10.0 Amper when ADC is 3.3V
        uint16_t curr = AdcPeriphery::get(AdcChannel::ADC_CURRENT);
        return curr * ADC_CURRENT_MULTIPLIER;
    }

    static float voltage_vin() {
        constexpr float ADC_VIN_MULTIPLIER = 1.0f / 64.0f;
        uint16_t volt = AdcPeriphery::get(AdcChannel::ADC_VIN);
        return volt * ADC_VIN_MULTIPLIER;
    }

    static float voltage_5v() {
        constexpr float ADC_5V_MULTIPLIER = 1.0f / 640.0f;
        uint16_t volt = AdcPeriphery::get(AdcChannel::ADC_5V);
        return volt * ADC_5V_MULTIPLIER;
    }
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
