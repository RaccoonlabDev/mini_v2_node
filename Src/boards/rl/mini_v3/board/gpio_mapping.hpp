/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#pragma once
#include "peripheral/gpio/gpio.hpp"

namespace BoardGpio {
static constexpr HAL::GpioPin INVALID = HAL::GPIO_INVALID_PIN;
static constexpr HAL::GpioPin CAN1_TERMINATOR = 0;
static constexpr HAL::GpioPin CAN2_TERMINATOR = 1;
}  // namespace BoardGpio
