/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_ALGORITHMS_H_
#define SRC_COMMON_ALGORITHMS_H_

#include <stdint.h>
#include <array>

typedef uint16_t PwmDurationUs;
static constexpr float PI_2 = 1.5707963267f;
static constexpr float LOOSE_ERR = 1e-3f;
static constexpr float PI = 3.1415926535f;

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

/**
 * @brief Normalizes a quaternion to unit length
 * @param q Quaternion to normalize in-place [x, y, z, w]
 */
void normalize_quaternion(std::array<float, 4>& q);

/**
 * @brief Converts quaternion to Euler angles (roll, pitch, yaw)
 * @param q Quaternion in [x, y, z, w] order (body frame)
 * @param angles_rpy Output array for [roll, pitch, yaw] in radians
 */
void quaternion_to_euler(const std::array<float, 4>& q,
                                std::array<float, 3>& angles_rpy);
/**
 * @brief The Adaptive Alpha Filter is a variation of the exponential smoothing filter,
 * where the smoothing factor α is adjusted dynamically based on the magnitude of changes
 * between consecutive inputs.
 */
class AdaptiveAlphaFilter {
public:
    AdaptiveAlphaFilter(float small_delta = 0.0f, float large_delta = 100.0f,
                        float smooth_alpha = 0.10f, float fast_alpha = 0.90f);

    float update(float new_value);

private:
    float linearly_interpolate_alpha(float delta) const;

    const float SMALL_DELTA;
    const float LARGE_DELTA;
    const float SMOOTH_ALPHA;
    const float FAST_ALPHA;

    float previous_filtered_value{0.0f};
};

#endif  // SRC_COMMON_ALGORITHMS_H_
