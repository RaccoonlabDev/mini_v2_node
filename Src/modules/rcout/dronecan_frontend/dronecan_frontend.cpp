/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_frontend.hpp"
#include "modules/rcout/router.hpp"

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

// Publish gimbal status with current servo angles in degrees
void DronecanPwmFrontend::publish_gimbal_status(uint16_t max_servos_angle) {

    gimbal_status_pub.msg.gimbal_id = 0;
    gimbal_status_pub.msg.mode.command_mode = 1; // example
    
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = 0; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = 0; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = 0; // Z
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[3] = 1; // W
    
    float roll_deg = 0, pitch_deg = 0, yaw_deg = 0;
    
    for (size_t i = 0; i < Driver::RCPWM::get_pins_amount(); ++i) {
        if (i > 3) {
            break; // Only first three servos are used for roll, pitch, yaw
        }
        switch (i) {
            case 0: // Roll
                roll_deg = static_cast<float>(Driver::RCPWM::get_current_angle(max_servos_angle, i));
                break;
            case 1: // Pitch
                pitch_deg = static_cast<float>(Driver::RCPWM::get_current_angle(max_servos_angle, i));
                break;
            case 2: // Yaw
                yaw_deg = static_cast<float>(Driver::RCPWM::get_current_angle(max_servos_angle, i));
                break;
            default:
                break;
        }
    }

    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = roll_deg; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = pitch_deg; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = yaw_deg; // Z

    gimbal_status_pub.publish();
}

std::array<float, 3> quaternion_to_euler(const std::array<float, 4>& q) {
    // q[0] = w, q[1] = x, q[2] = y, q[3] = z
    float w = q[0], x = q[1], y = q[2], z = q[3];
    
    std::array<float, 3> euler_angles{0.0f, 0.0f, 0.0f};
    
    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    euler_angles[0] = std::atan2(sinr_cosp, cosr_cosp);
    
    // Pitch (y-axis rotation)
    float sinp = 2.0f * (w * y - z * x);
    if (std::fabs(sinp) >= 1.0f) {
        // Use 90 degrees if out of range
        euler_angles[1] = std::copysign(M_PI / 2.0f, sinp);
    } else {
        euler_angles[1] = std::asin(sinp);
    }
    
    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    euler_angles[2] = std::atan2(siny_cosp, cosy_cosp);
    
    return euler_angles;
}

void DronecanPwmFrontend::set_gimbal_angles(const std::array<float, 4>& q, uint16_t max_servos_angle){
    float max_deflection = max_servos_angle / 2.0f;
    for (size_t i = 0; i < Driver::RCPWM::get_pins_amount(); ++i) {
        switch (Driver::RCPWM::get_pin_channel(i)) {
            case 0: // Roll
                Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[0] /max_deflection);
                break;
            case 1: // Pitch
                Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[1] / max_deflection);
                break;
            case 2: // Yaw
                Driver::RCPWM::channels[i].set_normalized_signed(angles_rpy[2] / max_deflection);
                break;
            default:
                break;
        }
    }
}
void DronecanPwmFrontend::arming_status_callback(const uavcan_equipment_safety_ArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with SafetyArmingStatus?
}
