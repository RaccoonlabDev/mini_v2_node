/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_frontend.hpp"
#include "modules/pwm/main.hpp"
#include "common/zip.hpp"

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

    for (auto&& [pwm, timing] : zip(Driver::RCPWM::channels, PWMModule::timings)) {
        if (pwm.channel >= msg.size || pwm.channel < 0) {
            continue;
        }

        auto cmd_int14 = msg.raw_cmd[pwm.channel];

        if (cmd_int14 < 0) {
            timing.set_standby_state();
            pwm.set_default();
        } else if (cmd_int14 == 0) {
            timing.set_default_state();
            pwm.set_percent(0);
        } else {
            timing.set_engaged_state();
            pwm.set_int14(cmd_int14);
        }
    }
}


void DronecanPwmFrontend::array_command_callback(const ArrayCommand_t& msg) {
    logger.log_debug("ac_cb");

    if (pwm_cmd_type != CommandType::ARRAY_COMMAND) {
        return;
    }

    for (auto&& [pwm, timing] : zip(Driver::RCPWM::channels, PWMModule::timings)) {
        if (pwm.channel < 0 || pwm.channel > 255) {
            continue;
        }

        for (uint8_t array_command_idx = 0; array_command_idx < msg.size; array_command_idx++) {
            if (msg.commads[array_command_idx].actuator_id != pwm.channel) {
                continue;
            }

            auto normalized_command = msg.commads[array_command_idx].command_value;

            bool is_engaged = normalized_command < -0.001f || normalized_command > 0.001f;
            if (is_engaged) {
                timing.set_engaged_state();
            } else {
                timing.set_default_state();
            }

            pwm.set_normalized_servo(normalized_command);
        }
    }
}

void DronecanPwmFrontend::hardpoint_callback(const HardpointCommand& msg) {
    logger.log_debug("hp_cb");

    if (pwm_cmd_type != CommandType::HARDPOINT_COMMAND) {
        return;
    }

    for (auto&& [pwm, timing] : zip(Driver::RCPWM::channels, PWMModule::timings)) {
        if (msg.hardpoint_id != pwm.channel) {
            continue;
        }

        auto cmd = msg.command;

        timing.set_engage_forever();  // TTL has no effect on Hardpoint Command

        pwm.set_percent(cmd == 1 ? 100 : 0);
    }
}

void DronecanPwmFrontend::arming_status_callback(const SafetyArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with SafetyArmingStatus?
}
