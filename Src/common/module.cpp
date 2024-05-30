/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "module.hpp"

BaseModule::BaseModule(float frequency) : period_ms(period_ms_from_frequency(frequency)) {
}

void BaseModule::init() {
    mode = ModuleMode::OPEARTIONAL;
}

ModuleStatus BaseModule::get_health() const {
    return health;
}

ModuleMode BaseModule::get_mode() const {
    return mode;
}

void BaseModule::process() {
    uint32_t crnt_time_ms = HAL_GetTick();
    if (crnt_time_ms < next_spin_time_ms) {
        return;
    }

    next_spin_time_ms = crnt_time_ms + period_ms;
    update_params();
    spin_once();
}

uint32_t BaseModule::period_ms_from_frequency(float frequency) {
    return (frequency > 0.001f) ? static_cast<uint32_t>(1000.0f / frequency) : 1000;
}
