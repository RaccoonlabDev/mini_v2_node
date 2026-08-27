/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_PERIPHERAL_GPIO_UBUNTU_HPP_
#define SRC_PERIPHERAL_GPIO_UBUNTU_HPP_

#include <span>
#include "peripheral/gpio/gpio.hpp"

namespace HAL {

extern std::span<bool> gpios;

}  // namespace HAL

#endif  // SRC_PERIPHERAL_GPIO_UBUNTU_HPP_
