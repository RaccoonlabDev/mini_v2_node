/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/system/main.hpp"
#include "common/algorithms.hpp"
#include "params.hpp"

REGISTER_MODULE(SystemModule)

void SystemModule::init() {
    need_notification = paramsGetIntegerValue(IntParamsIndexes::PARAM_LOG_LEVEL) <= 1;
    set_health(Status::OK);
    set_mode(Mode::STANDBY);
}

void SystemModule::spin_once() {
    if (!need_notification || HAL_GetTick() < 1000) {
        return;
    }

    need_notification = false;

    // Maximum expected firmware full info size
    constexpr auto MAX_SIZE = sizeof("Node v99.99.99_BADCOFFE RelWithDebInfo CLANG 999.999.999");
    static_assert(sizeof(FIRMWARE_FULL_INFO) < MAX_SIZE);
    logger.log_info(FIRMWARE_FULL_INFO);
}
