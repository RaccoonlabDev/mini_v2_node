/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRIVERS_RCPWM_RCPWM_HPP_
#define SRC_DRIVERS_RCPWM_RCPWM_HPP_

#include <cstdint>
#include "peripheral/pwm/pwm.hpp"

namespace Driver {

class RCPWM {
public:
    static constexpr uint8_t get_pins_amount() {
        return static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT);
    }
};

}  // namespace Driver

#endif  // SRC_DRIVERS_RCPWM_RCPWM_HPP_
