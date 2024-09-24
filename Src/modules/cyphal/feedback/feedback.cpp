/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "feedback.hpp"
#include <algorithm>
#include "cyphal.hpp"
#include "params.hpp"
#include "peripheral/pwm/pwm.hpp"

REGISTER_MODULE(CyphalFeedbackModule)

void FeedbackPublisher::publish() {
    uint8_t buffer[reg_udral_service_actuator_common_Feedback_0_1_EXTENT_BYTES_];
    size_t size = reg_udral_service_actuator_common_Feedback_0_1_EXTENT_BYTES_;
    int32_t res = reg_udral_service_actuator_common_Feedback_0_1_serialize_(&msg, buffer, &size);
    if (NUNAVUT_SUCCESS == res) {
        push(size, buffer);
    }
}

void CyphalFeedbackModule::update_params() {
    auto port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_1_ID));
    pub.setPortId(port_id);
}

void CyphalFeedbackModule::spin_once() {
    pub.msg.heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
    pub.msg.heartbeat.readiness.value = reg_udral_service_common_Readiness_0_1_ENGAGED;
    uint32_t pwm_ccr_reg_value = HAL::Pwm::get_duration(HAL::PwmPin::PWM_1);
    uint32_t pwm_duration = std::clamp(pwm_ccr_reg_value, (uint32_t)1000, (uint32_t)2000);
    pub.msg.demand_factor_pct = static_cast<int8_t>((pwm_duration - 1000) / 10);

    pub.publish();
}
