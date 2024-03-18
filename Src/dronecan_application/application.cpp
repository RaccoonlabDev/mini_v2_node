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
#include "periphery/pwm/pwm.hpp"
#include "modules/CircuitStatusModule.hpp"


void application_entry_point() {
    paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    auto node_id = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);

    const auto node_name = "co.raccoonlab.mini";
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    paramsSetStringValue(node_name_param_idx, 19, (const uint8_t*)node_name);
    uavcanSetNodeName(node_name);

    LedPeriphery::reset();
    uavcanInitApplication(node_id);
    PwmPeriphery::init(PwmPin::PWM_2);
    CircuitStatusModule& status_module = CircuitStatusModule::get_instance();
    LedColor color = LedColor::BLUE_COLOR;
    if (!status_module.instance_initialized){
        color = LedColor::RED_COLOR;
    }
    while(true) {
        LedPeriphery::toggle(color);
        status_module.spin_once();
        uavcanSpinOnce();
        PwmPeriphery::set_duration(PwmPin::PWM_2, 1000);

        WatchdogPeriphery::refresh();

    }
}
