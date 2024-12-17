/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "module.hpp"
#include <span>
#include "params.hpp"

Module::Module(float frequency, Protocol proto) : _period_ms(period_ms_from_frequency(frequency)),
                                                  _protocol(proto) {
    ModuleManager::register_module(this);
}

void Module::init() {
    _mode = Mode::STANDBY;
}

bool Module::is_enabled() const {
    auto active_protocol = ModuleManager::get_active_protocol();
    return _protocol == Protocol::CYPHAL_AND_DRONECAN || _protocol == active_protocol;
}

void Module::process() {
    uint32_t crnt_time_ms = HAL_GetTick();
    if (crnt_time_ms < _next_spin_time_ms) {
        return;
    }

    _next_spin_time_ms = crnt_time_ms + _period_ms;
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
}

void ModuleManager::init() {
    for (auto app_module : active_modules) {
        if (app_module->is_enabled()) {
            app_module->init();
        }
    }
}

void ModuleManager::process() {
    for (auto app_module : active_modules) {
        if (app_module->is_enabled()) {
            app_module->process();
        }
    }
}

Module::Protocol ModuleManager::get_active_protocol() {
#if defined(CONFIG_USE_CYPHAL) && !defined(CONFIG_USE_DRONECAN)
    return Module::Protocol::CYPHAL;
#elif !defined(CONFIG_USE_CYPHAL) && defined(CONFIG_USE_DRONECAN)
    return Module::Protocol::DRONECAN;
#else
    auto system_protocol = paramsGetIntegerValue(PARAM_SYSTEM_PROTOCOL);

    Module::Protocol protocol;
    if (system_protocol == static_cast<int32_t>(Module::Protocol::CYPHAL)) {
        protocol = Module::Protocol::CYPHAL;
    } else {
        protocol = Module::Protocol::DRONECAN;
    }

    return protocol;
#endif
}

Module::Status ModuleManager::get_global_status() {
    auto global_status = Module::Status::OK;

    for (auto app_module : active_modules) {
        if (app_module->is_enabled() && app_module->get_health() > global_status) {
            global_status = app_module->get_health();
        }
    }

    return global_status;
}

Module::Mode ModuleManager::get_global_mode() {
    auto global_mode = Module::Mode::STANDBY;

    for (auto app_module : active_modules) {
        if (app_module->is_enabled() && app_module->get_mode() > global_mode) {
            global_mode = app_module->get_mode();
        }
    }

    return global_mode;
}

uint8_t ModuleManager::get_vssc() {
    uint8_t vssc = 0;

    uint8_t module_idx = 0;
    for (auto app_module : active_modules) {
        if (!app_module->is_enabled()) {
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
