/*
 * Copyright (C) 2018-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file stm32_temperature.h
 * @author d.ponomarev
 * @date Nov 18, 2018
 */

#ifndef SRC_PERIPHERY_TEMPERATURE_SENSOR_HPP_
#define SRC_PERIPHERY_TEMPERATURE_SENSOR_HPP_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Get temperature value
* @return temperature value in degree celsius
*/
uint16_t stm32TemperatureParse(uint16_t raw_adc_value);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PERIPHERY_TEMPERATURE_SENSOR_HPP_
