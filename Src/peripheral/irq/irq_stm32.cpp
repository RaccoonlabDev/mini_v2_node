/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/irq/irq.hpp"

#include "main.h"

namespace HAL {

void platform_enable_irq() {
    __enable_irq();
}

}  // namespace HAL
