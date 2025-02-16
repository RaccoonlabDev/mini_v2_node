/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/gpio/gpio.hpp"
#include <vector>
#include "main.h"

namespace HAL {

static std::vector<bool> gpio((int)GPIO::Pin::GPIO_AMOUNT, false);

void GPIO::set(Pin gpio_pin, bool state) {
    gpio[(int)gpio_pin] = state;
}

bool GPIO::get(Pin gpio_pin) {
    return gpio[(int)gpio_pin];
}

}  // namespace HAL
