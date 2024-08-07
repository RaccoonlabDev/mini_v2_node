/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "cyphal_module.hpp"
#include <algorithm>
#include "cyphal.hpp"
#include "params.hpp"
#include "periphery/adc/circuit_periphery.hpp"

CyphalModule::CyphalModule() : Module(0) {
}

void CyphalModule::init() {
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
    paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);

    int param_node_id_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
    auto node_id = std::clamp(param_node_id_value, 1, 126);
    cyphal.init(node_id);

    cyphal::NodeGetInfoSubscriber::setHardwareVersion(2, 1);

    health = Status::OK;
    mode = Mode::OPEARTIONAL;
}

void CyphalModule::spin_once() {
    cyphal.setNodeHealth(uavcan_node_Health_1_0{(uint8_t)(Module::get_global_status())});
    cyphal.setNodeMode(uavcan_node_Mode_1_0{(uint8_t)(Module::get_global_mode())});
    cyphal.setVSSC(Module::get_vssc());
    cyphal.process();
}
