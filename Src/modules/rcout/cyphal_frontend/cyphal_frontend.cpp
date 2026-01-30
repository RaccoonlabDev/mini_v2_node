/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "cyphal_frontend.hpp"
#include <algorithm>
#include "libcpnode/cyphal.hpp"
#include "modules/rcout/router.hpp"
#include "params.hpp"
#include "drivers/rcpwm/rcpwm.hpp"

namespace {
void setpointCallback(reg_udral_service_actuator_common_sp_Vector31_0_1* msg) {
    const size_t max_setpoints = std::min<size_t>(Driver::RCPWM::get_pins_amount(), 31U);
    for (size_t setpoint_idx = 0; setpoint_idx < max_setpoints; setpoint_idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = static_cast<int16_t>(setpoint_idx);
        cmd.kind = CommandKind::NORMALIZED_UNSIGNED;
        cmd.value = msg->value[setpoint_idx];
        pwm_router.apply(cmd);
    }
}
}  // namespace

void CyphalPwmFrontend::init() {
    auto cyphal = libcpnode::Cyphal::get_instance();
    static auto setpoint_sub = cyphal->makeSubscriber<reg_udral_service_actuator_common_sp_Vector31_0_1>(
        setpointCallback,
        static_cast<uint16_t>(paramsGetIntegerValue(PARAM_SUB_SETPOINT_ID))
    );
    (void)setpoint_sub;
}

void CyphalPwmFrontend::update_params() {
    // nothing to do yet
}
