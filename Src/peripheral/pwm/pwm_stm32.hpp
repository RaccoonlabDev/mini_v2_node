/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_PWM_STM32_PWM_HPP_
#define SRC_PERIPHERAL_PWM_STM32_PWM_HPP_

#include <stdint.h>
#include <span>
#include "peripheral/pwm/pwm.hpp"
#include "main.h"

namespace HAL {

struct PwmPinInfo {
    TIM_HandleTypeDef& htim;
    uint32_t channel;
    volatile uint32_t& ccr;
};

extern const std::span<const PwmPinInfo> pwms;

}  // namespace HAL

#endif  // SRC_PERIPHERAL_PWM_STM32_PWM_HPP_
