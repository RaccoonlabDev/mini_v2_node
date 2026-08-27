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

using GpioPin = uint8_t;
inline constexpr GpioPin GPIO_INVALID_PIN = UINT8_MAX;

class GPIO {
public:
    static uint8_t pin_count();
    static void set(GpioPin gpio_pin, bool state);
    static bool get(GpioPin gpio_pin);

private:
    static bool is_valid_pin(GpioPin gpio_pin);
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_GPIO_HPP_
