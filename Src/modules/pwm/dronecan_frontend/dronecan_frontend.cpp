/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_frontend.hpp"
#include <limits>
#include "modules/pwm/main.hpp"

void DronecanPwmFrontend::init(PWMModule* backend_) {
    backend = backend_;

    raw_command_sub.init(raw_command_callback);
    array_command_sub.init(array_command_callback);
    hardpoint_sub.init(hardpoint_callback);
    arming_status_sub.init(arming_status_callback);
}

void DronecanPwmFrontend::update_params() {
    pwm_cmd_type = (CommandType)paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TYPE);
    if (pwm_cmd_type >= CommandType::NUMBER_OF_COMMANDS) {
        pwm_cmd_type = CommandType::RAW_COMMAND;
    }
}

void DronecanPwmFrontend::raw_command_callback(const RawCommand_t& msg) {
    logger.log_debug("rc_cb");

    if (pwm_cmd_type != CommandType::RAW_COMMAND) {
        return;
    }

    for (auto& pwm : PWMModule::params) {
        if (pwm.channel >= msg.size || pwm.channel < 0) {
            continue;
        }

        auto cmd = msg.raw_cmd[pwm.channel];
        pwm.last_recv_time_ms = HAL_GetTick();

        if (cmd < 0) {
            pwm.engaged_deadline_ms = 0;
            HAL::Pwm::set_duration(pwm.pin, pwm.def);
        } else if (cmd == 0) {
            HAL::Pwm::set_duration(pwm.pin, pwm.min);
        } else {
            pwm.engaged_deadline_ms = pwm.last_recv_time_ms + PWMModule::cmd_ttl;
            HAL::Pwm::set_duration(pwm.pin, mapInt16CommandToPwm(cmd, pwm.min, pwm.max, pwm.def));
        }
    }
}


void DronecanPwmFrontend::array_command_callback(const ArrayCommand_t& msg) {
    logger.log_debug("ac_cb");

    if (pwm_cmd_type != CommandType::ARRAY_COMMAND) {
        return;
    }

    for (auto& pwm : PWMModule::params) {
        if (pwm.channel < 0 || pwm.channel > 255) {
            continue;
        }

        for (uint8_t array_command_idx = 0; array_command_idx < msg.size; array_command_idx++) {
            if (msg.commads[array_command_idx].actuator_id != pwm.channel) {
                continue;
            }

            auto cmd = msg.commads[array_command_idx].command_value;
            pwm.last_recv_time_ms = HAL_GetTick();

            if (cmd < -0.001f || cmd > 0.001f) {
                pwm.engaged_deadline_ms = HAL_GetTick() + PWMModule::cmd_ttl;
            }

            HAL::Pwm::set_duration(pwm.pin, mapFloatCommandToPwm(cmd, pwm.min, pwm.max, pwm.def));
        }
    }
}

void DronecanPwmFrontend::hardpoint_callback(const HardpointCommand& msg) {
    logger.log_debug("hp_cb");

    if (pwm_cmd_type != CommandType::HARDPOINT_COMMAND) {
        return;
    }

    for (auto& pwm : PWMModule::params) {
        if (msg.hardpoint_id != pwm.channel) {
            continue;
        }

        auto cmd = msg.command;

        // TTL has no effect on Hardpoint Command, so we don't update it
        pwm.last_recv_time_ms = std::numeric_limits<uint32_t>::max();

        // Hardpoint doesn't support engaged state monitoring and then it is always engaged
        pwm.engaged_deadline_ms = std::numeric_limits<uint32_t>::max();

        HAL::Pwm::set_duration(pwm.pin, cmd == 1 ? pwm.max : pwm.min);
    }
}

void DronecanPwmFrontend::arming_status_callback(const ArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with ArmingStatus?
}
