/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "arming.hpp"
#include "params.hpp"
#include "common/algorithms.hpp"

REGISTER_MODULE(ArmingModule)

void ArmingModule::spin_once() {
    auto global_mode = ModuleManager::get_global_mode();
    auto crnt_time_ms = HAL_GetTick();
    if (global_mode == Mode::ENGAGED && !is_armed) {
        is_armed = true;
        arm_start_time = crnt_time_ms;
        return;
    }

    uint32_t elapsed_time_ms = crnt_time_ms - arm_start_time;
    if (is_armed && global_mode != Mode::ENGAGED) {
        is_armed = false;
        if (elapsed_time_ms > PERIOD_OF_INSENSITIVITY_MS) {
            uint32_t prev_eng_time = paramsGetIntegerValue(IntParamsIndexes::PARAM_STATS_ENG_TIME);
            auto new_eng_time = prev_eng_time + elapsed_time_ms / 1000;
            paramsSetIntegerValue(IntParamsIndexes::PARAM_STATS_ENG_TIME, int(new_eng_time));
            paramsSave();
            logger.log_info("Engaged time has been udpated");
        }
    }
}
