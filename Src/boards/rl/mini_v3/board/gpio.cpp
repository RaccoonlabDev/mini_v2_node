/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include <array>
#include <span>
#include "gpio_mapping.hpp"
#include "peripheral/gpio/gpio_stm32.hpp"

namespace HAL {
namespace {
const std::array<GpioPinInfo, 2> kGpios = {{
        {.port = CAN1_TERMINATOR_GPIO_Port, .pin = CAN1_TERMINATOR_Pin},
        {.port = CAN2_TERMINATOR_GPIO_Port, .pin = CAN2_TERMINATOR_Pin},
    }
};
static_assert(BoardGpio::CAN1_TERMINATOR == 0);
static_assert(BoardGpio::CAN2_TERMINATOR == 1);
static_assert(BoardGpio::CAN2_TERMINATOR + 1 == kGpios.size());
}

const std::span<const GpioPinInfo> gpios{kGpios};

}  // namespace HAL
