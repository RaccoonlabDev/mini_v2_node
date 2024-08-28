/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "arming.hpp"
#include <storage.h>
#include "can_driver.h"
#include "common/algorithms.hpp"
#include <params.hpp>
#include <logger.hpp>

Logger logger = Logger("ArmingModule");

REGISTER_MODULE(ArmingModule)
bool ArmingModule::is_armed = false;
uint32_t ArmingModule::arm_start_time = 0;
int64_t ArmingModule::disarm_time = -1;

void ArmingModule::init() {
    health = Status::OK;

    if (raw_command_sub.init(raw_command_cb) < 0) {
        health = Status::FATAL_MALFANCTION;
    }
    prev_eng_time = paramsGetIntegerValue(IntParamsIndexes::PARAM_STATS_ENG_TIME);

    mode = Module::Mode::STANDY;
}

void ArmingModule::spin_once() {
    if (!is_armed && disarm_time > 0) {
        auto cur_eng_time = (HAL_GetTick() - ArmingModule::arm_start_time) / 1000;
        prev_eng_time = cur_eng_time + prev_eng_time;
        paramsSetIntegerValue(IntParamsIndexes::PARAM_STATS_ENG_TIME, int(prev_eng_time));
        paramsSave();
        disarm_time = -1;
    }
}

void ArmingModule::raw_command_cb(const RawCommand_t& msg) {
    if (msg.size < RAW_COMMAND_CHANNEL + 1) {
        return;
    }
    auto raw_command_value = msg.raw_cmd[RAW_COMMAND_CHANNEL];

    if (!is_armed) {
        if (raw_command_value > -1){
            is_armed = true;
            if (disarm_time != -1) {
                disarm_time = -1;
                arm_start_time = HAL_GetTick();
            }
        }
        return;
    }

    // armed
    if (is_armed && raw_command_value < 0) {
        disarm_time = HAL_GetTick();
        is_armed = false;
    }
}
