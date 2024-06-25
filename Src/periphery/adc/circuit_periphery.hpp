// Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
#define SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_

#include <stdint.h>
#include <limits>
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

    /**
     * @return The current in Amperes if the hardware supports it, otherwise NaN.
     */
    static float current() {
        if (auto hw_version = hardware_version(); hw_version < 2403 || hw_version > 2450) {
            return std::numeric_limits<float>::quiet_NaN();
        }

        // Current sensor: INA169NA/3K, R = 33K ohm
        // Calibration coefficient was measured experimentally
        constexpr float MAX_SENSOR_CURRENT = 10.0f;
        constexpr float CALIBRATION_COEF = 0.6666667f;
        constexpr float ADC_CURRENT_MULTIPLIER = MAX_SENSOR_CURRENT * CALIBRATION_COEF / 4095.0f;
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

    static uint16_t hardware_version() {
        return AdcPeriphery::get(AdcChannel::ADC_VERSION);
    }
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
