/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "application.hpp"
#include "cyphal.hpp"
#include "cyphal_registers.hpp"
#include "main.h"
#include "params.hpp"
#include "setpoint/setpoint.hpp"
#include "feedback/feedback.hpp"
#include "circuit_status/circuit_status.hpp"
#include "periphery/led/led.hpp"
#include "periphery/iwdg/iwdg.hpp"

void init_persistent_storage() {
    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    paramsSetStringValue(node_name_param_idx, 19, (const uint8_t*)"co.raccoonlab.mini");
}

__attribute__((noreturn)) void application_entry_point() {
    LedPeriphery::reset();
    init_persistent_storage();
    cyphal::NodeGetInfoSubscriber::setHardwareVersion(2, 1);

    cyphal::Cyphal cyphal;
    cyphal.init();

    SetpointModule setpoint;
    FeedbackModule feedback;
    CircuitStatus crct;

    std::array<Module*, 3> modules = { &setpoint, &feedback, &crct };

    for (auto module : modules) {
        module->init();
    }

    while (true) {
        auto health = Module::Status::OK;
        auto mode = Module::Mode::OPEARTIONAL;

        cyphal.process();
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
        cyphal.setNodeHealth(uavcan_node_Health_1_0{static_cast<uint8_t>(health)});
        cyphal.setNodeMode(uavcan_node_Mode_1_0{static_cast<uint8_t>(mode)});
        LedPeriphery::toggle(color);
        WatchdogPeriphery::refresh();
    }
}
