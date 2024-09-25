/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/iwdg/iwdg.hpp"
#include "main.h"

namespace HAL {

static uint32_t watchdog_deadline_ms = 500;

void Watchdog::refresh() {
    if (!reboot_required) {
        watchdog_deadline_ms = platformSpecificGetTimeMs() + 500;
    }

    if (platformSpecificGetTimeMs() > watchdog_deadline_ms) {
        platformSpecificRebootForce();
    }
}

}  // namespace HAL
