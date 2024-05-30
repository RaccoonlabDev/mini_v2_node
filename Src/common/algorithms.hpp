/*
 * Copyright (C) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SRC_COMMON_ALGORITHMS_H_
#define SRC_COMMON_ALGORITHMS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t PwmDurationUs;

/**
 * @brief  Map raw command value (in interval from 0 to 8191)
 *         to PWM duration in us (in interval from min to max)
 * @return pwm_duration if input is correct,
 *         def_pwm if raw_command value is less than min or higher than max
 */
PwmDurationUs mapInt16CommandToPwm(int16_t rc_value,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm);

/**
 * @brief  Map array command value (in interval from -1.0 to 1.0)
 *         to PWM duration in us (in interval from min to max)
 * @return pwm_duration if input is correct,
 *         def_pwm if raw_command value is less than min or higher than max
 */
PwmDurationUs mapFloatCommandToPwm(float ac_value,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm);

/**
 * @brief  Map PWM duration in us (in interval from min to max) to pct
 * @return pwm_duration in pct
 */
float mapPwmToPct(uint16_t pwm_val, int16_t pwm_min, int16_t pwm_max);

float mapFloat(float value,
               float in_min, float in_max,
               float out_min, float out_max);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_ALGORITHMS_H_
