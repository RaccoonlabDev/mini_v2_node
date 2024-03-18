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

#define ADC_CURRENT_MULTIPLIER (10.0 / 4095)      // 10.0 Amper when ADC is 3.3V (4095)


class CircuitPeriphery{
public:
    AdcPeriphery adc;
    
    int8_t init() {
        int8_t adc_status = adc.init();
        return adc_status;
    };
    uint16_t internal_temp() {
        uint16_t temp = adc.get(AdcChannel::ADC_TEMPERATURE);
        return stm32TemperatureParse(temp);
    };
    float internal_curr() {
        uint16_t curr = adc.get(AdcChannel::ADC_CURRENT);
        return curr * ADC_CURRENT_MULTIPLIER;
    };
    float internal_volt() {
        uint16_t volt = adc.get(AdcChannel::ADC_VIN);
        return volt/64.0;
    };
    float internal_volt_5v() {
        uint16_t volt = adc.get(AdcChannel::ADC_5V);
        return volt/640.0;
    };
};


#ifdef __cplusplus
}
#endif

#endif  // SRC_DRIVER_CIRCUIT_PERIPHERY_HPP_
