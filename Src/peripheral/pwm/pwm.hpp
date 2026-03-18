/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_APPLICATION_PERIPHERY_PWM_HPP_
#define SRC_APPLICATION_PERIPHERY_PWM_HPP_

#include <stdint.h>

namespace HAL {

using PwmPin = uint8_t;
extern const uint8_t pwm_pins_amount;

class Pwm {
public:
    /**
     * @return 0 on success, otherwise < 0
     */
    static int8_t init(PwmPin pin);
    static uint8_t pin_count();

    /**
     * @brief Set the duration of the PWM signal for a specific PWM pin in microseconds
     */
    static void set_duration(const PwmPin pin, uint32_t duration_us);

    /**
     * @return the duration of the PWM signal for a specific PWM pin in microseconds
     */
    static uint32_t get_duration(PwmPin pin);

    /**
     * @return the duration of the PWM signal for a specific PWM pin in microseconds
     */
    static uint8_t get_percent(PwmPin pin, uint32_t min_duration_us, uint32_t max_duration_us);

    /**
     * @return the frequency of the PWM signal for a specific PWM pin in Hz
     */
    static uint32_t get_frequency(PwmPin pwm_pin);

    /**
     * @brief Set the frequency of the PWM signal for a specific PWM pin in Hz
     */
    static void set_frequency(PwmPin pwm_pin, uint32_t frequency_hz);

private:
    static bool is_valid_pin(PwmPin pin);
};

}  // namespace HAL

#endif  // SRC_APPLICATION_PERIPHERY_PWM_HPP_
