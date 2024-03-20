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
    paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    paramsSetStringValue(node_name_param_idx, 19, (const uint8_t*)"co.raccoonlab.mini");
}

__attribute__((noreturn)) void application_entry_point() {
    LedPeriphery::reset();
    init_persistent_storage();
    cyphal::NodeGetInfoSubscriber::setHardwareVersion(2, 1);

    cyphal::Cyphal cyphal;
    int init_res = cyphal.init();

    SetpointSubscriber setpoint(&cyphal);
    init_res |= setpoint.init();

    FeedbackPublisher feedback(&cyphal);
    init_res |= feedback.init();

    CircuitStatus crct;
    init_res |= crct.init();

    while (true) {
        auto led_color = (init_res >= 0) ? LedColor::BLUE_COLOR : LedColor::RED_COLOR;
        LedPeriphery::toggle(led_color);

        cyphal.process();

        auto crnt_time_ms = HAL_GetTick();
        feedback.process(crnt_time_ms);
        crct.process(crnt_time_ms);

        WatchdogPeriphery::refresh();
    }
}
