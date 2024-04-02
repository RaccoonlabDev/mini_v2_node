/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "feedback.hpp"
#include <algorithm>
#include "reg/udral/service/actuator/common/Feedback_0_1.h"
#include "cyphal.hpp"
#include "params.hpp"
#include "periphery/pwm/pwm.hpp"

int8_t FeedbackPublisher::init() {
    return 0;
}

void FeedbackPublisher::process(uint32_t crnt_time_ms) {
    if (_prev_pub_ts_ms + 1000 > crnt_time_ms) {
        return;
    }

    publish_msg(crnt_time_ms);
}

void FeedbackPublisher::publish_msg(uint32_t crnt_time_ms) {
    _prev_pub_ts_ms = crnt_time_ms;

    reg_udral_service_actuator_common_Feedback_0_1 msg;

    msg.heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
    msg.heartbeat.readiness.value = reg_udral_service_common_Readiness_0_1_ENGAGED;

    uint32_t pwm_ccr_reg_value = PwmPeriphery::get_duration(PwmPin::PWM_1);
    uint32_t pwm_duration = std::clamp(pwm_ccr_reg_value, (uint32_t)1000, (uint32_t)2000);
    msg.demand_factor_pct = static_cast<int8_t>((pwm_duration - 1000) / 10);
    setPortId(static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_FEEDBACK_1_ID)));

    uint8_t buffer[reg_udral_service_actuator_common_Feedback_0_1_EXTENT_BYTES_];
    size_t size = reg_udral_service_actuator_common_Feedback_0_1_EXTENT_BYTES_;
    int32_t res = reg_udral_service_actuator_common_Feedback_0_1_serialize_(&msg, buffer, &size);
    if (NUNAVUT_SUCCESS == res) {
        push(size, buffer);
    }
}
