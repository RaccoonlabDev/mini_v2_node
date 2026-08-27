/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include "peripheral/gpio/gpio_stm32.hpp"
#include <array>

namespace HAL {

namespace {
const std::array<GpioPinInfo, 0> kNoGpios{};
}

extern const std::span<const GpioPinInfo> gpios __attribute__((weak)) = kNoGpios;

bool GPIO::is_valid_pin(GpioPin gpio_pin) {
    return gpio_pin < pin_count();
}

uint8_t GPIO::pin_count() {
    return static_cast<uint8_t>(gpios.size());
}

void GPIO::set(GpioPin gpio_pin, bool state) {
    if (!is_valid_pin(gpio_pin)) {
        return;
    }

    const auto pin_state = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(gpios[gpio_pin].port, gpios[gpio_pin].pin, pin_state);
}

bool GPIO::get(GpioPin gpio_pin) {
    if (!is_valid_pin(gpio_pin)) {
        return false;
    }

    return HAL_GPIO_ReadPin(gpios[gpio_pin].port, gpios[gpio_pin].pin) == GPIO_PIN_SET;
}

}  // namespace HAL
