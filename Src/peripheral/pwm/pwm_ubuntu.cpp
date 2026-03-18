/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/pwm/pwm.hpp"
#include <cmath>
#include <span>
#include "common/algorithms.hpp"

namespace HAL {

extern std::span<uint32_t> pwm;
extern std::span<uint32_t> pwm_freq;

bool Pwm::is_valid_pin(PwmPin pin) {
    return pin < pin_count();
}

uint8_t Pwm::pin_count() {
    return pwm_pins_amount;
}

int8_t Pwm::init(PwmPin pwm_pin) {
    return is_valid_pin(pwm_pin) ? 0 : -1;
}

void Pwm::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    if (!is_valid_pin(pwm_pin)) {
        return;
    }
    pwm[pwm_pin] = duration_us;
}

uint32_t Pwm::get_duration(PwmPin pwm_pin) {
    return is_valid_pin(pwm_pin) ? pwm[pwm_pin] : 0;
}

uint8_t Pwm::get_percent(PwmPin pin, uint32_t min_duration_us, uint32_t max_duration_us) {
    const float pct = mapPwmToPct(get_duration(pin), min_duration_us, max_duration_us);
    return static_cast<uint8_t>(std::lround(pct));
}

void Pwm::set_frequency(const PwmPin pwm_pin, uint32_t frequency_hz) {
    if (!is_valid_pin(pwm_pin)) {
        return;
    }
    pwm_freq[pwm_pin] = frequency_hz;
}

uint32_t Pwm::get_frequency(PwmPin pwm_pin) {
    return is_valid_pin(pwm_pin) ? pwm_freq[pwm_pin] : 0;
}

}  // namespace HAL
