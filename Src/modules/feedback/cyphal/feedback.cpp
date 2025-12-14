/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "feedback.hpp"
#include <algorithm>
#include "libcpnode/cyphal.hpp"
#include "params.hpp"
#include "peripheral/adc/circuit_periphery.hpp"

REGISTER_MODULE(CyphalFeedbackModule)


void CyphalFeedbackModule::update_params() {
    udral_feedbacks[0].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_1_ID)));
    udral_feedbacks[1].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_2_ID)));
    udral_feedbacks[2].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_3_ID)));
    udral_feedbacks[3].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_4_ID)));

    compact_feedbacks[0].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_COMPACT_FEEDBACK_1_ID)));
    compact_feedbacks[1].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_COMPACT_FEEDBACK_2_ID)));
    compact_feedbacks[2].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_COMPACT_FEEDBACK_3_ID)));
    compact_feedbacks[3].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_COMPACT_FEEDBACK_4_ID)));
}

void CyphalFeedbackModule::spin_once() {
    for (size_t pin_idx = 0; pin_idx < Driver::RCPWM::get_pins_amount(); pin_idx++) {
        auto& udral_feedback = udral_feedbacks[pin_idx];
        if (!udral_feedback.isEnabled()) {
            continue;
        }

        udral_feedback.msg.heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
        udral_feedback.msg.heartbeat.readiness.value = reg_udral_service_common_Readiness_0_1_ENGAGED;
        udral_feedback.msg.demand_factor_pct = Driver::RCPWM::get_pin_percent(pin_idx);
        udral_feedback.publish();
    }

    for (size_t pin_idx = 0; pin_idx < Driver::RCPWM::get_pins_amount(); pin_idx++) {
        auto& compact_feedback = compact_feedbacks[pin_idx];
        if (!compact_feedback.isEnabled()) {
            continue;
        }

        compact_feedback.msg = {
            .dc_voltage = CircuitPeriphery::voltage_vin(),
            .dc_current = CircuitPeriphery::current(),
            .phase_current_amplitude = CircuitPeriphery::current(),
            .velocity = 0,
            .demand_factor_pct = (int8_t)Driver::RCPWM::get_pin_percent(pin_idx),
        };
        compact_feedback.publish();
    }
}
