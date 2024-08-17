/*
 * Copyright (C) 2022-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
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
 * @brief Maps a 16-bit integer command to a PWM duration.
 *
 * This function takes a 16-bit integer command and maps it to a PWM duration in microseconds.
 * If the command is outside the valid range [0, 8191], the default PWM duration is returned.
 * Otherwise, the command is linearly mapped to the specified PWM range.
 *
 * @note Suitable for esc.RawCommand mapping
 */
PwmDurationUs mapInt16CommandToPwm(int16_t rc_value,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm);

/**
 * @brief Maps a float command to a PWM duration.
 *
 * This function takes a float command and maps it to a PWM duration in microseconds.
 * If the command is outside the valid range [-1.0, 1.0], the default PWM duration is returned.
 * Otherwise, the command is linearly mapped to the specified PWM range.
 *
 * @note Suitable for actuator.Command or ratiometric setpoint mapping
 */
PwmDurationUs mapFloatCommandToPwm(float ac_value,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm);

/**
 * @brief Maps a value from one range to another.
 *
 * This function takes an input float value and linearly maps it from an input range
 * [in_min, in_max] to an output range [out_min, out_max]. If the input value is outside
 * the input range, the output value will be clamped to the nearest boundary of the output range.
 */
float mapFloat(float value,
               float in_min, float in_max,
               float out_min, float out_max);

/**
 * @brief  Map PWM duration in us (in interval from min to max) to pct
 * @return pwm_duration in pct
 */
float mapPwmToPct(uint16_t pwm_val, int16_t pwm_min, int16_t pwm_max);

void movingAverage(float* prev_avg, float crnt_val, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_ALGORITHMS_H_
