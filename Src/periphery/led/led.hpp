/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
#define SRC_APPLICATION_PERIPHERY_LED_LED_HPP_

#include <stdint.h>

namespace Board {

class Led {
public:
    enum class SimpleColor {
        RED_COLOR,
        GREEN_COLOR,
        BLUE_COLOR,
        COLORS_AMOUNT,
    };

    enum class Color : uint8_t {
        BLACK,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        YELLOW,
        WHITE,
        COLORS_AMOUNT,
    };

    /**
     * @brief Set the specific RGB LED color 
     */
    static void set(Color color);

    /**
     * @brief Turn off the RGB LED
     */
    static void reset();

    /**
     * @brief Blink LED with 50% duty cycle
     */
    static void blink(Color first, Color second);
};

}  // namespace Board


#endif  // SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
