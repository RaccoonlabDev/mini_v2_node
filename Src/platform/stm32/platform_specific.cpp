/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <string.h>
#include "main.h"
#include "peripheral/iwdg/iwdg.hpp"

namespace {

constexpr uint32_t RESTART_DELAY_MS = 500;
struct RestartContext {
    static inline bool restart_requested = false;
    static inline uint32_t restart_deadline_ms = 0;
};

void process_deferred_restart(uint32_t now_ms) {
    if (RestartContext::restart_requested
        && static_cast<int32_t>(now_ms - RestartContext::restart_deadline_ms) >= 0) {
        HAL_NVIC_SystemReset();
    }
}

}  // namespace

#ifdef __cplusplus
extern "C" {
#endif

void platformSpecificReadUniqueID(uint8_t out_uid[4]) {
    const uint32_t UNIQUE_ID_16_BYTES[4] = {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2(),
        0
    };
    memcpy(out_uid, UNIQUE_ID_16_BYTES, 16);
}

bool platformSpecificRequestRestart() {
    auto now_ms = HAL_GetTick();
    RestartContext::restart_deadline_ms = now_ms + RESTART_DELAY_MS;
    RestartContext::restart_requested = true;

#ifdef HAL_IWDG_MODULE_ENABLED
    HAL::Watchdog::request_reboot();
#endif
    return true;
}

void platformSpecificRebootForce() {
    HAL_NVIC_SystemReset();
}

uint32_t platformSpecificGetTimeMs() {
    auto now_ms = HAL_GetTick();
    process_deferred_restart(now_ms);
    return now_ms;
}

#ifdef __cplusplus
}
#endif
