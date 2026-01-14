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

void rad_to_deg_array(float angles_rpy[3]) {
    constexpr float RAD_TO_DEG = 57.2957795131f;  // 180.0f / PI
    angles_rpy[0] *= RAD_TO_DEG;
    angles_rpy[1] *= RAD_TO_DEG;
    angles_rpy[2] *= RAD_TO_DEG;
}

float fast_atan2(float y, float x) {
    if (std::abs(x) < FLT_EPSILON_LOCAL && std::abs(y) < FLT_EPSILON_LOCAL) {
        return 0.0f;
    }

    float angle = 0.0f;
    if (x < 0.0f) {
        angle = (y >= 0.0f) ? PI : -PI;
        // Effectively rotating by 180 degrees
        x = -x;
        y = -y;
    }

    // Iterative CORDIC rotation
    for (int i = 0; i < 12; i++) {
        float x_new;
        float sigma = (y >= 0.0f) ? 1.0f : -1.0f;
        float factor = std::ldexp(1.0f, -i);  // 2^-i

        x_new = x + (sigma * y * factor);
        y = y - (sigma * x * factor);
        x = x_new;

        angle += sigma * CORDIC_TABLE[i];
    }

    return angle;
}

// ZYX "decoding" (yaw-pitch-roll) convention
void quaternion_to_euler(const float q[4],
                         float angles_rpy[3]) {
    const float qx = q[0];
    const float qy = q[1];
    const float qz = q[2];
    const float qw = q[3];

    // Calculate the Sine of Pitch
    float sinp = 2.0f * (qw * qy - qx * qz);

    // Prevents NaN in sqrt/atan2 if the quaternion is slightly denormalized
    if (sinp > 1.0f) sinp = 1.0f;
    else if (sinp < -1.0f) sinp = -1.0f;

    // Gimbal Lock Check
    if (std::abs(sinp) > 0.9995f) {
        angles_rpy[0] = 0.0f;
        angles_rpy[1] = std::copysign(PI_2, sinp);
        angles_rpy[2] = fast_atan2(2.0f * (qx * qy + qw * qz),
                                   1.0f - 2.0f * (qy * qy + qz * qz));
    } else {
        // Roll (x-axis)
        angles_rpy[0] = fast_atan2(2.0f * (qw * qx + qy * qz),
                                   1.0f - 2.0f * (qx * qx + qy * qy));

        // Pitch (y-axis)
        float cosp = std::sqrt(1.0f - sinp * sinp);
        angles_rpy[1] = fast_atan2(sinp, cosp);

        // Yaw (z-axis)
        angles_rpy[2] = fast_atan2(2.0f * (qw * qz + qx * qy),
                                   1.0f - 2.0f * (qy * qy + qz * qz));
    }
}

void normalize_quaternion(float q[4]) {
    float norm_sq = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];

    if (norm_sq > 0.0f && std::fabs(norm_sq - 1.0f) > 1e-6f) {
        float inv_norm = 1.0f / std::sqrt(norm_sq);
        q[0] *= inv_norm;
        q[1] *= inv_norm;
        q[2] *= inv_norm;
        q[3] *= inv_norm;
    }
}
