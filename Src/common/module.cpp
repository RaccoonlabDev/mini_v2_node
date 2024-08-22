/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "module.hpp"
#include <span>

Module::Module(float frequency, Protocol proto) : protocol(proto),
                                                  period_ms(period_ms_from_frequency(frequency)) {
    ModuleManager::register_module(this);
}

void Module::init() {
    mode = Mode::STANDY;
}

Module::Status Module::get_health() const {
    return health;
}

Module::Mode Module::get_mode() const {
    return mode;
}

Module::Protocol Module::get_protocol() const {
    return protocol;
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

void ModuleManager::register_module(Module* app_module) {
    if (modules_amount < MAX_MODULES_AMOUNT) {
        modules[modules_amount] = app_module;
        modules_amount++;
        active_modules = std::span<Module*>(modules.data(), modules_amount);
    }
};

void ModuleManager::init(Module::Protocol proto) {
    protocol = proto;
    for (auto app_module : active_modules) {
        if (app_module->get_protocol() == protocol) {
            app_module->init();
        }
    }
};

void ModuleManager::process() {
    for (auto app_module : active_modules) {
        if (app_module->get_protocol() == protocol) {
            app_module->process();
        }
    }
};

Module::Status ModuleManager::get_global_status() {
    auto global_status = Module::Status::OK;

    for (auto app_module : active_modules) {
        if (app_module->get_protocol() == protocol && app_module->get_health() > global_status) {
            global_status = app_module->get_health();
        }
    }

    return global_status;
}

Module::Mode ModuleManager::get_global_mode() {
    auto global_mode = Module::Mode::STANDY;

    for (auto app_module : active_modules) {
        if (app_module->get_protocol() == protocol && app_module->get_mode() > global_mode) {
            global_mode = app_module->get_mode();
        }
    }

    return global_mode;
}

uint8_t ModuleManager::get_vssc() {
    uint8_t vssc = 0;

    uint8_t module_idx = 0;
    for (auto app_module : active_modules) {
        if (app_module->get_protocol() != protocol) {
            continue;
        }

        auto is_health_bad = app_module->get_health() > Module::Status::OK;
        auto is_mode_not_operational = app_module->get_mode() > Module::Mode::ENGAGED;
        if (is_health_bad || is_mode_not_operational) {
            vssc += 1 << module_idx;
        }
        module_idx++;
    }

    return vssc;
}
