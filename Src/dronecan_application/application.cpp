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
#include "modules/pwm/PWMModule.hpp"
#include "modules/circuit_status/CircuitStatusModule.hpp"


__attribute__((noreturn)) void application_entry_point() {
    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    auto node_id = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);

    const auto node_name = "co.raccoonlab.mini";
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    paramsSetStringValue(node_name_param_idx, 19, (const uint8_t*)node_name);
    uavcanSetNodeName(node_name);

    LedPeriphery::reset();

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
