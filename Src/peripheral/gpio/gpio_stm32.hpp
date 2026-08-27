/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_PERIPHERAL_GPIO_STM32_HPP_
#define SRC_PERIPHERAL_GPIO_STM32_HPP_

#include <span>
#include "main.h"
#include "peripheral/gpio/gpio.hpp"

namespace HAL {

struct GpioPinInfo {
    GPIO_TypeDef* port;
    uint16_t pin;
};

extern const std::span<const GpioPinInfo> gpios;

}  // namespace HAL

#endif  // SRC_PERIPHERAL_GPIO_STM32_HPP_
