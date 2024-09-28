/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "cyphal_frontend.hpp"
#include "reg/udral/service/actuator/common/sp/Vector31_0_1.h"
#include "common/algorithms.hpp"

#include "cyphal.hpp"
#include "cyphal_subscribers.hpp"

class SetpointSubscriber: public cyphal::CyphalSubscriber {
public:
    SetpointSubscriber() : CyphalSubscriber(cyphal::Cyphal::get_instance(), 65535) {}
    int8_t init();
private:
    void callback(const cyphal::CanardRxTransfer& transfer) override;
};
static SetpointSubscriber setpoint_sub;


void CyphalPwmFrontend::init(PWMModule* backend_) {
    backend = backend_;
    setpoint_sub.init();
}

int8_t SetpointSubscriber::init() {
    port_id = static_cast<uint16_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_SUB_SETPOINT_ID));
    if (driver->subscribe(this,
                reg_udral_service_actuator_common_sp_Vector31_0_1_EXTENT_BYTES_,
                cyphal::CanardTransferKindMessage) < 0) {
        return -1;
    }

    return 0;
}

void SetpointSubscriber::callback(const cyphal::CanardRxTransfer& transfer) {
    auto payload = static_cast<const uint8_t*>(transfer.payload);
    size_t len = transfer.payload_size;
    reg_udral_service_actuator_common_sp_Vector31_0_1 msg;
    if (reg_udral_service_actuator_common_sp_Vector31_0_1_deserialize_(&msg, payload, &len) < 0) {
        return;
    }

    int16_t number_of_setpoints = len / 2;

    for (auto& pwm : PWMModule::params) {
        if (pwm.channel >= number_of_setpoints) {
            continue;
        }

        auto value = std::clamp(msg.value[pwm.channel], 0.0f, 1.0f);
        pwm.last_recv_time_ms = HAL_GetTick();

        if (value > 0) {
            pwm.engaged_deadline_ms = HAL_GetTick() + PWMModule::cmd_ttl;
        }

        auto pwm_duration = (uint32_t)mapFloat(value, 0.0f, +1.0f, pwm.min, pwm.max);
        HAL::Pwm::set_duration(pwm.pin, pwm_duration);
    }
}
