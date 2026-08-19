/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/iwdg/iwdg.hpp"
#include "iwdg.h"

#ifdef HAL_IWDG_MODULE_ENABLED
#if defined(STM32H753xx)
    #define IWDG_HANDLE hiwdg1
#else
    #define IWDG_HANDLE hiwdg
#endif
#endif  // HAL_IWDG_MODULE_ENABLED

#ifdef FDCAN1
    extern FDCAN_HandleTypeDef hfdcan1;
#endif  // FDCAN1

#ifdef FDCAN2
    extern FDCAN_HandleTypeDef hfdcan2;
#endif  // FDCAN2

#if defined(FDCAN1) || defined(FDCAN2)
static bool is_bus_off(FDCAN_HandleTypeDef& handle) {
    FDCAN_ProtocolStatusTypeDef status{};

    if (HAL_FDCAN_GetProtocolStatus(&handle, &status) != HAL_OK) {
        return true;
    }

    return status.BusOff != 0U;
}
#endif

namespace HAL {

void Watchdog::refresh() {
#ifdef HAL_IWDG_MODULE_ENABLED
    if (reboot_required) {
        return;
    }

    bool can_is_healthy = true;

#ifdef FDCAN1
    can_is_healthy = can_is_healthy && !is_bus_off(hfdcan1);
#endif

#ifdef FDCAN2
    can_is_healthy = can_is_healthy && !is_bus_off(hfdcan2);
#endif

    if (can_is_healthy) {
        HAL_IWDG_Refresh(&IWDG_HANDLE);
    }
#endif  // HAL_IWDG_MODULE_ENABLED
}

}  // namespace HAL
