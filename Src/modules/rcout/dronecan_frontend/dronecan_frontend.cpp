/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "dronecan_frontend.hpp"
#include "modules/rcout/router.hpp"
#include "common/algorithms.hpp"
#include "common/logging.hpp"

void DronecanPwmFrontend::init() {
    uint8_t counterInit = 0;
    uint8_t counterFail = 0;
    raw_command_sub.init(raw_command_callback) >= 0 ? 
            counterInit++ : counterFail++;

    array_command_sub.init(array_command_callback)  >= 0 ? 
            counterInit++ : counterFail++;

    hardpoint_sub.init(hardpoint_callback) >= 0 ? 
            counterInit++ : counterFail++;

    arming_status_sub.init(arming_status_callback)  >= 0 ? 
            counterInit++ : counterFail++;

    gimbal_angular_command_sub.init(gimbal_angular_command_callback)  >= 0 ? 
            counterInit++ : counterFail++;
    char buffer[35];
    sprintf(buffer, "Rcout subs init: %d, subs failed: %d", counterInit, counterFail);
    logger.log_info(buffer);
}

void DronecanPwmFrontend::update_params() {
    pwm_cmd_type = (CommandType)paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_INPUT_TYPE);
    if (pwm_cmd_type >= CommandType::NUMBER_OF_COMMANDS) {
        pwm_cmd_type = CommandType::RAW_COMMAND;
    }
    gimbal::gimbal_id = static_cast<uint8_t>
        (paramsGetIntegerValue(IntParamsIndexes::PARAM_GIMBAL_ID));
    gimbal::max_servos_travel = paramsGetIntegerValue(IntParamsIndexes::PARAM_SERVO_MAX_TRAVEL_DEG);
}

void DronecanPwmFrontend::gimbal_angular_command_callback(const uavcan_equipment_camera_gimbal_AngularCommand& msg) {
    if (pwm_cmd_type != CommandType::GIMBAL_COMMAND) return;
    // If node receives a command for a different gimbal, ignore it
    if (msg.gimbal_id != gimbal::gimbal_id) {
        return;
    }
    gimbal::set_gimbal_state(msg.quaternion_xyzw);
}

void DronecanPwmFrontend::raw_command_callback(const uavcan_equipment_esc_RawCommand& msg) {
    if (pwm_cmd_type != CommandType::RAW_COMMAND) return;

    for (uint8_t idx = 0; idx < msg.cmd.len; idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = idx;
        cmd.kind = CommandKind::SIGNED_INT14;

        if (auto cmd_int14 = msg.cmd.data[idx]; cmd_int14 < 0) {
            cmd.force_default = true;
        } else {
            cmd.value = static_cast<float>(cmd_int14);
        }

        pwm_router.apply(cmd);
    }
}

void DronecanPwmFrontend::array_command_callback(const uavcan_equipment_actuator_ArrayCommand& msg) {
    if (pwm_cmd_type != CommandType::ARRAY_COMMAND) return;

    for (uint8_t array_command_idx = 0; array_command_idx < msg.commands.len; array_command_idx++) {
        ActuatorCommand cmd{};
        cmd.actuator_id = msg.commands.data[array_command_idx].actuator_id;
        cmd.kind = CommandKind::NORMALIZED_SIGNED;
        cmd.value = msg.commands.data[array_command_idx].command_value;
        pwm_router.apply(cmd);
    }
}

void DronecanPwmFrontend::hardpoint_callback(const uavcan_equipment_hardpoint_Command& msg) {
    if (pwm_cmd_type != CommandType::HARDPOINT_COMMAND) return;

    ActuatorCommand cmd{};
    cmd.actuator_id = msg.hardpoint_id;
    cmd.kind = CommandKind::BOOL;
    cmd.value = msg.command;
    cmd.engage_forever = true;

    pwm_router.apply(cmd);
}

void DronecanPwmFrontend::arming_status_callback(const uavcan_equipment_safety_ArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with SafetyArmingStatus?
}

namespace gimbal {
    float gimbal_q[4];
    uint16_t max_servos_travel = 90;
    uint8_t gimbal_id{0};
    
    void set_gimbal_state_rpy(const float angles_rpy[3]) {
        float max_deflection = max_servos_travel / 2.0f;
        for (size_t i = 0; i < Driver::RCPWM::get_pins_amount(); ++i) {
            switch (Driver::RCPWM::get_pin_channel(i)) {
                case 0: // Roll
                    Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[0] / max_deflection);
                    RcoutModule::timings[i].mark_command_fresh();
                    break;
                case 1: // Pitch
                    Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[1] / max_deflection);
                    RcoutModule::timings[i].mark_command_fresh();
                    break;
                case 2: // Yaw
                    Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[2] / max_deflection);
                    RcoutModule::timings[i].mark_command_fresh();
                    break;
                default:
                    break;
            }
        }
    }

    // Modifies the internal quaternion state
    void set_gimbal_state(const float q[4]) {
        float angles_rpy[3] = {0.0f, 0.0f, 0.0f};
        float q_copy[4] = {q[0], q[1], q[2], q[3]};
        normalize_quaternion(q_copy);
        quaternion_to_euler(q_copy, angles_rpy);
        rad_to_deg_array(angles_rpy);
        set_gimbal_state_rpy(angles_rpy);
    }
    
    const float* get_quaternion() {
        // Update gimbal_q before returning
        float angles_rpy[3] = {0.0f, 0.0f, 0.0f};
        for (size_t i = 0; i < Driver::RCPWM::get_pins_amount(); ++i) {
            switch (Driver::RCPWM::get_pin_channel(i)) {
                case 0: // Roll
                    angles_rpy[0] = Driver::RCPWM::get_current_angle(max_servos_travel, i);
                    break;
                case 1: // Pitch
                    angles_rpy[1] = Driver::RCPWM::get_current_angle(max_servos_travel, i);
                    break;
                case 2: // Yaw
                    angles_rpy[2] = Driver::RCPWM::get_current_angle(max_servos_travel, i);
                    break;
                default:
                    break;
            }
        }
        euler_to_quaternion(angles_rpy, gimbal_q);
        return gimbal_q;
    }
    
    void set_max_servos_angle(uint16_t angle) {
        max_servos_travel = angle;
    }
    
    uint16_t get_max_servos_angle() {
        return max_servos_travel;
    }
}