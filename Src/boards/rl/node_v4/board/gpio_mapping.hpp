/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_BOARDS_RL_NODE_V4_GPIO_MAPPING_HPP_
#define SRC_BOARDS_RL_NODE_V4_GPIO_MAPPING_HPP_

#include "peripheral/gpio/gpio.hpp"

namespace BoardGpio {

static constexpr HAL::GPIO::Pin INVALID = HAL::GPIO::Pin::GPIO_AMOUNT;

static constexpr HAL::GPIO::Pin INTERNAL_LED_RED = HAL::GPIO::Pin::INTERNAL_LED_RED;
static constexpr HAL::GPIO::Pin INTERNAL_LED_GREEN = HAL::GPIO::Pin::INTERNAL_LED_GREEN;
static constexpr HAL::GPIO::Pin INTERNAL_LED_BLUE = HAL::GPIO::Pin::INTERNAL_LED_BLUE;

static constexpr HAL::GPIO::Pin EXT_RGB_LED_RED = INVALID;
static constexpr HAL::GPIO::Pin EXT_RGB_LED_GREEN = INVALID;
static constexpr HAL::GPIO::Pin EXT_RGB_LED_BLUE = INVALID;

static constexpr HAL::GPIO::Pin CAN1_TERMINATOR = INVALID;
static constexpr HAL::GPIO::Pin CAN2_TERMINATOR = INVALID;

}  // namespace BoardGpio

#endif  // SRC_BOARDS_RL_NODE_V4_GPIO_MAPPING_HPP_
