/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/iwdg/iwdg.hpp"
#include "main.h"

#ifdef HAL_IWDG_MODULE_ENABLED
extern IWDG_HandleTypeDef hiwdg;
#endif  // HAL_IWDG_MODULE_ENABLED

#ifdef FDCAN1
    extern FDCAN_HandleTypeDef hfdcan1;
#endif  // FDCAN1

namespace HAL {

void Watchdog::refresh() {
#ifdef HAL_IWDG_MODULE_ENABLED
    if (reboot_required) {
        return;
    }
#endif  // HAL_IWDG_MODULE_ENABLED

#if defined(HAL_IWDG_MODULE_ENABLED) && defined(FDCAN1)
    if (!__HAL_FDCAN_GET_FLAG(&hfdcan1, FDCAN_FLAG_BUS_OFF)) {
        HAL_IWDG_Refresh(&hiwdg);
    }
#elif defined(HAL_IWDG_MODULE_ENABLED)
    HAL_IWDG_Refresh(&hiwdg);
#endif  // HAL_IWDG_MODULE_ENABLED
}

}  // namespace HAL
