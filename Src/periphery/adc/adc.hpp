// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_PERIPHERY_ADC_ADC_HPP_
#define SRC_PERIPHERY_ADC_ADC_HPP_

#include <stdint.h>
#include "periphery/temperature_sensor/temperature_sensor.hpp"
#ifdef __cplusplus
extern "C" {
#endif

#define ADC_CURRENT_MULTIPLIER 0.04884004884         // 200.0 Amper when ADC is 3.3V (4095)

enum class AdcChannel : uint8_t {
    ADC_VIN, 
    ADC_5V,
    ADC_CURRENT,
    ADC_VERSION,
    ADC_TEMPERATURE,
    ADC_NUMBER_OF_CNANNELS,
};

class AdcPeriphery {
public:
    static int8_t init();
    static uint16_t get(AdcChannel channel);
    static float stm32Temperature(uint16_t temp);
    static float stm32Current(uint16_t curr);
    static float stm32Voltage(uint16_t volt);
    static float stm32Voltage5V(uint16_t volt);
private:
    static inline bool _is_adc_already_inited = false;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_PERIPHERY_ADC_ADC_HPP_
