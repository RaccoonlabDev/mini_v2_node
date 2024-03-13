/*
 * Copyright (C) 2018-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file stm32_temperature.c
 * @author d.ponomarev
 * @brief Implementation of TemperatureSensor
 * @date Nov 18, 2018
 * @note From the datasheets:
 * mcu          temp_ref, C     v_ref,V     avg_slope,mV/C
 * stm32f103    25              1.41        4.3
 * stm32g0      30              0.76        2.5
 */

#include "periphery/temperature_sensor/temperature_sensor.hpp"

#ifdef STM32G0B1xx
    static const uint16_t TEMP_REF = 30;
    static const uint16_t ADC_REF = 943;    ///< v_ref / 3.3 * 4095
    static const uint16_t AVG_SLOPE = 3.1;  ///< avg_slope/(3.3/4096)
#else  // STM32F103xB
    static const uint16_t TEMP_REF = 25;
    static const uint16_t ADC_REF = 1750;   ///< v_ref / 3.3 * 4095
    static const uint16_t AVG_SLOPE = 5;    ///< avg_slope/(3.3/4096)
#endif

uint16_t stm32TemperatureParse(uint16_t adc_measurement) {
    uint16_t temperature = (ADC_REF - adc_measurement) / AVG_SLOPE + TEMP_REF + 273;
    return temperature;
}