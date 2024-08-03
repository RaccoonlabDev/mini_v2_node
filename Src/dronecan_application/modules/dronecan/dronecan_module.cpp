/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_module.hpp"
#include <algorithm>
#include "params.hpp"
#include "dronecan.h"
#include "periphery/adc/circuit_periphery.hpp"

DronecanModule::DronecanModule() : Module(0) {
}

void DronecanModule::init() {
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
    paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);
    uavcanSetNodeName(board_name);

    int param_node_id_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
    auto node_id = std::clamp(param_node_id_value, 1, 126);
    uavcanInitApplication(node_id);

    health = Status::OK;
    mode = Mode::OPEARTIONAL;
}

void DronecanModule::spin_once() {
    uavcanSetNodeHealth(static_cast<NodeStatusHealth_t>(Module::get_global_status()));
    uavcanSetNodeStatusMode(static_cast<NodeStatusMode_t>(Module::get_global_mode()));
    uavcanSetVendorSpecificStatusCode(Module::get_vssc());
    uavcanSpinOnce();
}
