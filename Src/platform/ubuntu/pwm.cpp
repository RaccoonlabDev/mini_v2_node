/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/pwm/pwm.hpp"

int8_t PwmPeriphery::init(PwmPin pwm_pin) {
    (void)pwm_pin;
    return 0;
}

void PwmPeriphery::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    (void)pwm_pin;
    (void)duration_us;

}

uint32_t PwmPeriphery::get_duration(PwmPin pwm_pin) {
    (void)pwm_pin;
    return 0;
}
