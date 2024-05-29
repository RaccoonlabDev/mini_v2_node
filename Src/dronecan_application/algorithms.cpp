/*
 * Copyright (C) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "algorithms.hpp"
#include <math.h>

static const RawCommand RAW_COMMAND_MIN = 0;
static const RawCommand RAW_COMMAND_MAX = 8191;


static const ActuatorCommandValue ACT_COMMAND_MIN = -1.0f;
static const ActuatorCommandValue ACT_COMMAND_MAX = 1.0f;

PwmDurationUs mapRawCommandToPwm(RawCommand command,
                                 PwmDurationUs min_pwm,
                                 PwmDurationUs max_pwm,
                                 PwmDurationUs def_pwm) {
    PwmDurationUs pwm;
    if (command < RAW_COMMAND_MIN || command > RAW_COMMAND_MAX) {
        pwm = def_pwm;
    } else {
        pwm = (PwmDurationUs)mapFloat(command, RAW_COMMAND_MIN, RAW_COMMAND_MAX, min_pwm, max_pwm);
    }
    return pwm;
}


PwmDurationUs mapActuatorCommandToPwm(ActuatorCommandValue command,
                                 PwmDurationUs min_pwm,
                                 PwmDurationUs max_pwm,
                                 PwmDurationUs def_pwm) {
    PwmDurationUs pwm;
    if (command < ACT_COMMAND_MIN || command > ACT_COMMAND_MAX) {
        pwm = def_pwm;
    } else {
        pwm = (PwmDurationUs)mapFloat(command, ACT_COMMAND_MIN, ACT_COMMAND_MAX, min_pwm, max_pwm);
    }
    return pwm;
}

float mapPwmToPct(uint16_t pwm_val, int16_t pwm_min, int16_t pwm_max) {
    auto max = pwm_max;
    auto min = pwm_min;
    if (pwm_min > pwm_max) {
        max = pwm_min;
        min = pwm_max;
    }
    auto scaled_val = (pwm_val - min) * 100.0f / (max - min);
    auto val = std::clamp(scaled_val, 0.f, 100.f);
    return val;
}

float mapFloat(float value, float in_min, float in_max, float out_min, float out_max) {
    float output;
    if (value <= in_min && in_min <= in_max) {
        output = out_min;
    } else if (value >= in_max && in_min <= in_max) {
        output = out_max;
    } else if (fabs(out_min - out_max) < 0.001) {
        output = out_min;
    } else {
        output = out_min + (value - in_min) / (in_max - in_min) * (out_max - out_min);
        if (out_min <= out_max) {
            output = std::clamp(output, out_min, out_max);
        } else {
            output = std::clamp(output, out_max, out_min);
        }
    }
    return output;
}
