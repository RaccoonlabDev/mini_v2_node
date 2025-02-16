/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/gpio/gpio.hpp"
#include "main.h"

namespace HAL {

void GPIO::set(Pin gpio_pin, bool state) {
#ifdef CAN1_TERMINATOR_GPIO_Port
    auto pin_state = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    switch (gpio_pin) {
        case Pin::CAN1_TERMINATOR:
            HAL_GPIO_WritePin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin, pin_state);
            break;

        case Pin::CAN2_TERMINATOR:
            HAL_GPIO_WritePin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin, pin_state);
            break;
        default:
            break;
    }
#else
    (void)gpio_pin;
    (void)state;
#endif
}

bool GPIO::get(Pin gpio_pin) {
#ifdef CAN1_TERMINATOR_GPIO_Port
    switch (gpio_pin) {
        case Pin::CAN1_TERMINATOR:
            return HAL_GPIO_ReadPin(CAN1_TERMINATOR_GPIO_Port, CAN1_TERMINATOR_Pin);

        case Pin::CAN2_TERMINATOR:
            return HAL_GPIO_ReadPin(CAN2_TERMINATOR_GPIO_Port, CAN2_TERMINATOR_Pin);

        default:
            return GPIO_PIN_SET;
    }
#else
    (void)gpio_pin;
    return GPIO_PIN_RESET;
#endif
}

}  // namespace HAL
