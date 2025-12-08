/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_frontend.hpp"
#include "modules/pwm/main.hpp"
#include "modules/pwm/router.hpp"

void DronecanPwmFrontend::init() {
    raw_command_sub.init(raw_command_callback);
    array_command_sub.init(array_command_callback);
    hardpoint_sub.init(hardpoint_callback);
    arming_status_sub.init(arming_status_callback);
}

void DronecanPwmFrontend::update_params() {
    pwm_cmd_type = (CommandType)paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_INPUT_TYPE);
    if (pwm_cmd_type >= CommandType::NUMBER_OF_COMMANDS) {
        pwm_cmd_type = CommandType::RAW_COMMAND;
    }
}

void DronecanPwmFrontend::raw_command_callback(const RawCommand_t& msg) {
    if (pwm_cmd_type != CommandType::RAW_COMMAND) return;

    for (uint8_t idx = 0; idx < msg.size; idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = idx;
        cmd.kind = CommandKind::SIGNED_INT14;

        auto cmd_int14 = msg.raw_cmd[idx];
        if (cmd_int14 < 0) {
            cmd.force_default = true;
        } else {
            cmd.value = static_cast<float>(cmd_int14);
        }

        pwm_router.apply(cmd);
    }
}

void DronecanPwmFrontend::array_command_callback(const ArrayCommand_t& msg) {
    if (pwm_cmd_type != CommandType::ARRAY_COMMAND) return;

    for (uint8_t array_command_idx = 0; array_command_idx < msg.size; array_command_idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = msg.commads[array_command_idx].actuator_id;
        cmd.kind = CommandKind::NORMALIZED_SIGNED;
        cmd.value = msg.commads[array_command_idx].command_value;
        pwm_router.apply(cmd);
    }
}

void DronecanPwmFrontend::hardpoint_callback(const HardpointCommand& msg) {
    if (pwm_cmd_type != CommandType::HARDPOINT_COMMAND) return;

    ActuatorCommand cmd{};
    cmd.actuator_id = msg.hardpoint_id;
    cmd.kind = CommandKind::BOOL;
    cmd.value = msg.command;
    cmd.engage_forever = true;

    pwm_router.apply(cmd);
}

void DronecanPwmFrontend::arming_status_callback(const SafetyArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with SafetyArmingStatus?
}
