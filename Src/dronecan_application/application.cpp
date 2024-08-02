/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "application.hpp"
#include "dronecan.h"
#include "main.h"
#include "params.hpp"
#include "periphery/led/led.hpp"
#include "periphery/iwdg/iwdg.hpp"
#include "periphery/adc/circuit_periphery.hpp"
#include "modules/pwm/PWMModule.hpp"
#include "modules/circuit_status/CircuitStatusModule.hpp"
#include "common.hpp"

static void get_node_info() {
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    auto name = (const char*)paramsGetStringValue(node_name_param_idx);

    if (name[0] < 'a' || name[9] > 'z') {
        const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
        paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);
        uavcanSetNodeName(board_name);
    } else {
        uavcanSetNodeName(name);
    }
}

__attribute__((noreturn)) void application_entry_point() {
    init_board_periphery();

    auto node_id = get_node_id();
    get_node_info();

    uavcanInitApplication(node_id);

    CircuitStatus circuit_status;
    PWMModule pwm_module;

    std::array<Module*, 2> modules = { &circuit_status, &pwm_module };

    for (auto module : modules) {
        module->init();
    }
    while(true) {
        auto health = Module::Status::OK;
        auto mode = Module::Mode::OPEARTIONAL;
        for (auto module : modules) {
            module->process();

            if (module->get_health() > health) {
                health = module->get_health();
            }

            if (module->get_mode() > mode) {
                mode = module->get_mode();
            }
        }

        auto color = (health == Module::Status::OK) ? LedColor::BLUE_COLOR : LedColor::RED_COLOR;
        LedPeriphery::toggle(color);
        uavcanSetNodeHealth((NodeStatusHealth_t)(health));
        uavcanSetNodeStatusMode((NodeStatusMode_t)(mode));
        uavcanSpinOnce();

        WatchdogPeriphery::refresh();
    }
}
