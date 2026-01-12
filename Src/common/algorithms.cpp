/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "algorithms.hpp"
#include <math.h>
#include <algorithm>
#include <array>

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

void movingAverage(float* prev_avg, float crnt_val, uint16_t size) {
    if (prev_avg != nullptr && size != 0) {
        *prev_avg = (*prev_avg * (size - 1) + crnt_val) / size;
    }
}


AdaptiveAlphaFilter::AdaptiveAlphaFilter(float small_delta, float large_delta,
                                         float smooth_alpha, float fast_alpha) :
    SMALL_DELTA(small_delta),
    LARGE_DELTA(large_delta),
    SMOOTH_ALPHA(smooth_alpha),
    FAST_ALPHA(fast_alpha) {
}

float AdaptiveAlphaFilter::update(float new_value) {
    float delta = new_value - previous_filtered_value;
    float alpha = linearly_interpolate_alpha(delta);
    previous_filtered_value += alpha * delta;
    return previous_filtered_value;
}

float AdaptiveAlphaFilter::linearly_interpolate_alpha(float delta) const {
    auto abs_delta = fabs(delta);
    float alpha;
    if (abs_delta <= SMALL_DELTA) {
        alpha = SMOOTH_ALPHA;
    } else if (abs_delta >= LARGE_DELTA) {
        alpha = FAST_ALPHA;
    } else {
        float fraction = (abs_delta - SMALL_DELTA) / (LARGE_DELTA - SMALL_DELTA);
        alpha = SMOOTH_ALPHA + fraction * (FAST_ALPHA - SMOOTH_ALPHA);
    }

    return alpha;
}

void quaternion_to_euler(const std::array<float, 4>& q,
                                std::array<float, 3>& angles_rpy) {
    const float qx = q[0];
    const float qy = q[1];
    const float qz = q[2];
    const float qw = q[3];

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (qw * qx + qy * qz);
    double cosr_cosp = 1 - 2 * (qx * qx + qy * qy);
    angles_rpy[0] = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = std::sqrt(1 + 2 * (qw * qy - qx * qz));
    double cosp = std::sqrt(1 - 2 * (qw * qy - qx * qz));
    angles_rpy[1] = 2 * std::atan2(sinp, cosp) - PI_2;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (qw * qz + qx * qy);
    double cosy_cosp = 1 - 2 * (qy * qy + qz * qz);
    angles_rpy[2] = std::atan2(siny_cosp, cosy_cosp);
}


void normalize_quaternion(std::array<float, 4>& q) {
    float norm_sq = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];

    if (norm_sq > 0.0f && std::fabs(norm_sq - 1.0f) > 1e-6f) {
        float inv_norm = 1.0f / std::sqrt(norm_sq);
        q[0] *= inv_norm;
        q[1] *= inv_norm;
        q[2] *= inv_norm;
        q[3] *= inv_norm;
    }
}
