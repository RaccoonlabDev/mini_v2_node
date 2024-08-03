/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "module.hpp"
#include "modules/modules.hpp"

Module::Module(float frequency) : period_ms(period_ms_from_frequency(frequency)) {
}

void Module::init() {
    mode = Mode::OPEARTIONAL;
}

Module::Status Module::get_health() const {
    return health;
}

Module::Mode Module::get_mode() const {
    return mode;
}

void Module::process() {
    uint32_t crnt_time_ms = HAL_GetTick();
    if (crnt_time_ms < next_spin_time_ms) {
        return;
    }

    next_spin_time_ms = crnt_time_ms + period_ms;
    update_params();
    spin_once();
}

uint32_t Module::period_ms_from_frequency(float frequency) {
    return (frequency > 0.001f) ? static_cast<uint32_t>(1000.0f / frequency) : 0;
}

Module::Status Module::get_global_status() {
    const auto& modules = get_application_modules();
    auto global_status = Module::Status::OK;

    for (auto app_module : modules) {
        if (app_module->get_health() > global_status) {
            global_status = app_module->get_health();
        }
    }

    return global_status;
}

Module::Mode Module::get_global_mode() {
    const auto& modules = get_application_modules();
    auto global_mode = Module::Mode::OPEARTIONAL;

    for (auto app_module : modules) {
        if (app_module->get_mode() > global_mode) {
            global_mode = app_module->get_mode();
        }
    }

    return global_mode;
}

uint8_t Module::get_vssc() {
    const auto& modules = get_application_modules();
    uint8_t vssc = 0;

    uint8_t module_idx = 0;
    for (auto app_module : modules) {
        if (app_module->get_health() > Status::OK || app_module->get_mode() > Mode::OPEARTIONAL) {
            vssc += 1 << module_idx;
        }
        module_idx++;
    }

    return vssc;
}
