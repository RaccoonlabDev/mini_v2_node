/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/led/led.hpp"

namespace Board {

void Led::set(Led::Color color) {
    (void)color;
}

void Led::reset() {
    // not nothing
}

void Led::blink(Led::Color first, Led::Color second) {
    (void)first;
    (void)second;
}

}  // namespace Board
