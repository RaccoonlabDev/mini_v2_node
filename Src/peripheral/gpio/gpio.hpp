/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_GPIO_HPP_
#define SRC_PERIPHERAL_GPIO_HPP_

#include <stdint.h>

namespace HAL {

class GPIO {
public:
    enum class Pin {
        /**
        * @brief Any node must have an internal RGB LED
        */
        INTERNAL_LED_RED,
        INTERNAL_LED_GREEN,
        INTERNAL_LED_BLUE,

        /**
        * @brief A node may have an auxilliary external RGB LED
        */
        EXT_RGB_LED_RED,
        EXT_RGB_LED_GREEN,
        EXT_RGB_LED_BLUE,

        /**
        * @brief CAN terminator 120 ohm resistor
        * v2: doesn't exist yet
        * v3: PA15
        */
        CAN1_TERMINATOR,

        /**
        * @brief CAN terminator 120 ohm resistor
        * v2: doesn't exist yet
        * v3: PB15
        */
        CAN2_TERMINATOR,

        GPIO_AMOUNT,
    };

    static void set(const Pin gpio_pin, bool state);
    static bool get(const Pin gpio_pin);
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_GPIO_HPP_
