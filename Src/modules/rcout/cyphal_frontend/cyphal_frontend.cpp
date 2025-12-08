/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "cyphal_frontend.hpp"
#include "reg/udral/service/actuator/common/sp/Vector31_0_1.h"
#include "modules/rcout/router.hpp"

#include "cyphalNode/cyphal.hpp"

class SetpointSubscriber: public cyphal::CyphalSubscriber {
public:
    SetpointSubscriber() : CyphalSubscriber(cyphal::Cyphal::get_instance(), 65535) {}
    int8_t init();
private:
    void callback(const cyphal::CanardRxTransfer& transfer) override;
};
static SetpointSubscriber setpoint_sub;


void CyphalPwmFrontend::init() {
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

    for (uint8_t setpoint_idx = 0; setpoint_idx < number_of_setpoints; setpoint_idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = setpoint_idx;
        cmd.kind = CommandKind::NORMALIZED_UNSIGNED;
        cmd.value = msg.value[setpoint_idx];
        pwm_router.apply(cmd);
    }
}

void CyphalPwmFrontend::update_params() {
}