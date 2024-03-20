/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "setpoint.hpp"
#include <algorithm>
#include "reg/udral/service/actuator/common/sp/Vector31_0_1.h"
#include "cyphal.hpp"
#include "params.hpp"
#include "periphery/pwm/pwm.hpp"

SetpointSubscriber::SetpointSubscriber(cyphal::Cyphal* driver_) :
    CyphalSubscriber(driver_, 0) {
};

int8_t SetpointSubscriber::init() {
    PwmPeriphery::init(PwmPin::PWM_1);
    PwmPeriphery::init(PwmPin::PWM_2);
    PwmPeriphery::init(PwmPin::PWM_3);
    PwmPeriphery::init(PwmPin::PWM_4);

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
    size_t payload_len = transfer.payload_size;
    reg_udral_service_actuator_common_sp_Vector31_0_1 msg;
    if (reg_udral_service_actuator_common_sp_Vector31_0_1_deserialize_(&msg, payload, &payload_len) < 0) {
        return;
    }

    float setpoint_clamped = std::clamp(msg.value[0], 0.0f, 1.0f);
    auto pwm_duration = static_cast<uint32_t>(setpoint_clamped * 1000.0 + 1000);
    PwmPeriphery::set_duration(PwmPin::PWM_1, pwm_duration);
}
