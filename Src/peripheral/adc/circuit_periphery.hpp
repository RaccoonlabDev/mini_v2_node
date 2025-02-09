// Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
#define SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_

#include <stdint.h>
#include <limits>
#include <utility>
#include "peripheral/adc/adc.hpp"

#ifdef __cplusplus
extern "C" {
#endif

enum class BoardType : uint8_t {
    POWER_V3_1_0,
    ASPD_V3,
    RANGE_V3_1_0,
    KIRPI_NO_CURRENT,
    KIRPI_WITH_CURRENT,
    MINI_V2_1_1,
    MINI_V3_0_0,

    BOARDS_AMOUNT,
};

class CircuitPeriphery{
public:
    static int8_t init(){
        return HAL::Adc::init();
    }

    /**
     * @return Temperature, Kelvin
     */
    static uint16_t temperature();

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
        uint16_t curr = HAL::Adc::get(HAL::AdcChannel::ADC_CURRENT);
        return curr * ADC_CURRENT_MULTIPLIER;
    }

    static float voltage_vin() {
        constexpr float ADC_VIN_MULTIPLIER = 1.0f / 64.0f;
        uint16_t volt = HAL::Adc::get(HAL::AdcChannel::ADC_VIN);
        return volt * ADC_VIN_MULTIPLIER;
    }

    static float voltage_5v() {
        constexpr float ADC_5V_MULTIPLIER = 1.0f / 640.0f;
        uint16_t volt = HAL::Adc::get(HAL::AdcChannel::ADC_5V);
        return volt * ADC_5V_MULTIPLIER;
    }

    static uint16_t hardware_version() {
        return HAL::Adc::get(HAL::AdcChannel::ADC_VERSION);
    }

    static bool overvoltage() {
        return voltage_5v() > 5.5 || voltage_vin() > 60.0;
    }

    static bool undervoltage() {
        return voltage_5v() < 4.2 || voltage_vin() < 4.5;
    }

    static bool overcurrent() {
        return current() > 60.0;
    }

    static bool overheat() {
        return temperature() > (273.5f + 80.0f);
    }

    static bool is_failure() {
        return overvoltage() || undervoltage() || overcurrent() || overheat();
    }

    static BoardType detect_board_type();
    static std::pair<const char*, uint8_t> get_board_name();
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
