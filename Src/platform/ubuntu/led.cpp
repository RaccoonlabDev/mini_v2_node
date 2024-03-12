/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/led/led.hpp"

void LedPeriphery::reset() {
    // not nothing
}

void LedPeriphery::toggle(LedColor led_color) {
    (void)led_color;
}
