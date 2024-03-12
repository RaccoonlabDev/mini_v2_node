/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
#define SRC_APPLICATION_PERIPHERY_LED_LED_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum class LedColor {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
    COLORS_AMOUNT,
};

class LedPeriphery {
public:
    /**
     * @brief Set the specific RGB LED color 
     */
    static void set(LedColor color);

    /**
     * @brief Turn off the RGB LED
     */
    static void reset();

    /**
     * @brief Toggle the state of an RGB LED by alternating between setting
     * the LED to a specific color and turning it off.
     * @note If the current time is within the first set the given color,
     * otherwise turn off the RGB LED
     */
    static void toggle(LedColor led_color);
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
