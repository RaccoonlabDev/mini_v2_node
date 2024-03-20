// Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_PERIPHERY_ADC_ADC_HPP_
#define SRC_PERIPHERY_ADC_ADC_HPP_

#include <stdint.h>
#include "periphery/temperature_sensor/temperature_sensor.hpp"
#include "periphery/temperature_sensor/temperature_sensor.hpp"
#ifdef __cplusplus
extern "C" {
#endif

#define ADC_CURRENT_MULTIPLIER (10.0 / 4095)      // 10.0 Amper when ADC is 3.3V (4095)

#define ADC_CURRENT_MULTIPLIER (10.0 / 4095)      // 10.0 Amper when ADC is 3.3V (4095)

enum class AdcChannel : uint8_t {
    ADC_VIN, 
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
    
    
private:
    static inline bool _is_adc_already_inited = false;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_PERIPHERY_ADC_ADC_HPP_
