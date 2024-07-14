/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/pwm/pwm.hpp"

namespace Peripheral {

uint32_t pwm[(int)PwmPin::PWM_AMOUNT];
uint32_t pwm_freq[(int)PwmPin::PWM_AMOUNT];

int8_t Pwm::init(PwmPin pwm_pin) {
    (void)pwm_pin;
    return 0;
}

void Pwm::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    pwm[(int)pwm_pin] = duration_us;
}

uint32_t Pwm::get_duration(PwmPin pwm_pin) {
    return pwm[(int)pwm_pin];
}

void Pwm::set_frequency(const PwmPin pwm_pin, uint32_t frequency_hz) {
    pwm_freq[(int) pwm_pin] = frequency_hz;
}

uint32_t Pwm::get_frequency(PwmPin pwm_pin) {
    return pwm_freq[(int)pwm_pin];
}

}  // namespace Peripheral
