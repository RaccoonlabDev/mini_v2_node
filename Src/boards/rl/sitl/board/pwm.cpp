/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include <array>
#include <span>
#include "peripheral/pwm/pwm.hpp"

namespace HAL {

namespace {

std::array<uint32_t, 4> PWM{};
std::array<uint32_t, 4> PWM_FREQ{};

}  // namespace

const uint8_t pwm_pins_amount = static_cast<uint8_t>(PWM.size());
std::span<uint32_t> pwm{PWM};
std::span<uint32_t> pwm_freq{PWM_FREQ};

}  // namespace HAL
