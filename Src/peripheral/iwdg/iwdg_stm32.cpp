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

namespace HAL {

#if defined(HAL_IWDG_MODULE_ENABLED) && defined(STM32H753xx)
namespace {
// Stop extending the watchdog after this much time in one flash transaction.
constexpr uint32_t FLASH_WATCHDOG_MAX_HEARTBEAT_MS = 3000U;
// while flash is busy, refresh IWDG every 100 ms
constexpr uint32_t FLASH_WATCHDOG_REFRESH_PERIOD_MS = 100U;
constexpr uint32_t FLASH_WATCHDOG_EFFECTIVE_REFRESH_PERIOD_MS =
    (FLASH_WATCHDOG_MAX_HEARTBEAT_MS < FLASH_WATCHDOG_REFRESH_PERIOD_MS)
        ? FLASH_WATCHDOG_MAX_HEARTBEAT_MS
        : FLASH_WATCHDOG_REFRESH_PERIOD_MS;
static_assert(FLASH_WATCHDOG_MAX_HEARTBEAT_MS > 0U);
static_assert(FLASH_WATCHDOG_REFRESH_PERIOD_MS > 0U);
uint32_t last_flash_watchdog_refresh_ms = 0U;
}  // namespace
#endif

void Watchdog::refresh() {
#ifdef HAL_IWDG_MODULE_ENABLED
    if (reboot_required) {
        return;
    }
#endif  // HAL_IWDG_MODULE_ENABLED

#if defined(HAL_IWDG_MODULE_ENABLED) && defined(STM32H753xx)
    HAL_IWDG_Refresh(&IWDG_HANDLE);
#elif defined(HAL_IWDG_MODULE_ENABLED) && defined(FDCAN1)
    if (!__HAL_FDCAN_GET_FLAG(&hfdcan1, FDCAN_FLAG_BUS_OFF)) {
        HAL_IWDG_Refresh(&IWDG_HANDLE);
    }
#elif defined(HAL_IWDG_MODULE_ENABLED)
    HAL_IWDG_Refresh(&IWDG_HANDLE);
#endif  // HAL_IWDG_MODULE_ENABLED
}

}  // namespace HAL

extern "C" void flashOperationHeartbeatBegin() {
#if defined(HAL_IWDG_MODULE_ENABLED) && defined(STM32H753xx)
    HAL::last_flash_watchdog_refresh_ms =
        HAL_GetTick() - HAL::FLASH_WATCHDOG_EFFECTIVE_REFRESH_PERIOD_MS;
#endif
}

extern "C" void flashOperationHeartbeat(uint32_t elapsed_ms) {
#if defined(HAL_IWDG_MODULE_ENABLED) && defined(STM32H753xx)
    if (HAL::Watchdog::is_reboot_requested()) {
        return;
    }
    if (elapsed_ms > HAL::FLASH_WATCHDOG_MAX_HEARTBEAT_MS) {
        return;
    }

    const uint32_t now_ms = HAL_GetTick();
    if ((now_ms - HAL::last_flash_watchdog_refresh_ms) >=
            HAL::FLASH_WATCHDOG_EFFECTIVE_REFRESH_PERIOD_MS) {
        HAL_IWDG_Refresh(&IWDG_HANDLE);
        HAL::last_flash_watchdog_refresh_ms = now_ms;
    }
#else
    (void)elapsed_ms;
#endif
}

extern "C" void flashOperationHeartbeatEnd() {
}
