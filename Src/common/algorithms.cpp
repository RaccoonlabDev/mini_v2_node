/*
 * Copyright (C) 2022-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "algorithms.hpp"
#include <math.h>
#include <algorithm>


PwmDurationUs mapInt16CommandToPwm(int16_t command,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm) {
    PwmDurationUs pwm;
    if (command < 0 || command > 8191) {
        pwm = def_pwm;
    } else {
        pwm = (PwmDurationUs)mapFloat((float)command, 0.0f, 8191.0f, min_pwm, max_pwm);
    }
    return pwm;
}


PwmDurationUs mapFloatCommandToPwm(float command,
                                   PwmDurationUs min_pwm,
                                   PwmDurationUs max_pwm,
                                   PwmDurationUs def_pwm) {
    PwmDurationUs pwm;
    if (command < -1.0f || command > +1.0f) {
        pwm = def_pwm;
    } else {
        pwm = (PwmDurationUs)mapFloat(command, -1.0f, +1.0f, min_pwm, max_pwm);
    }
    return pwm;
}

float mapFloat(float value, float in_min, float in_max, float out_min, float out_max) {
    if (std::fabs(in_min - in_max) < 1e-6f) {
        return out_min;
    }

    float output = out_min + (value - in_min) / (in_max - in_min) * (out_max - out_min);

    return std::clamp(output, std::min(out_min, out_max), std::max(out_min, out_max));
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
