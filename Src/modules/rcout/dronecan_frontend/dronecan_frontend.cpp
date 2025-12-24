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

    // Callbacks are still stubs
    angular_command_sub.init(angular_command_callback);
    ahrs_solution_sub.init(ahrs_solution_callback);
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

void DronecanPwmFrontend::angular_command_callback(
    const uavcan_equipment_camera_gimbal_AngularCommand& msg) {
    logger.log_debug("AngularCommand received");
    
    // msg.gimbal_id = which gimbal (usually 0)
    // msg.mode = how to interpret quaternion
    // msg.quaternion_xyzw = target orientation or angular velocity
    
    if (msg.gimbal_id != 0) {
        return;  // Not for our gimbal
    }
    
    // Store target attitude for gimbal control
    // backend->target_attitude = msg.quaternion_xyzw;
    // Use switch to handle gimbal modes
}

		
void DronecanPwmFrontend::publish_gimbal_status(uint8_t servo_coefficient_100us) {

    gimbal_status_pub.msg.gimbal_id = 0;
    gimbal_status_pub.msg.mode.command_mode = 1; // example
    
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = 0; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = 0; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = 0; // Z
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[3] = 1; // W
    
    float roll_deg = 0, pitch_deg = 0, yaw_deg = 0;
    
    for (size_t i = 0; i < Driver::RCPWM::channels.size(); ++i) {
        auto& ch = Driver::RCPWM::channels[i];
        
        if (ch.channel < 0 || ch.channel > 2) continue; // Not a gimbal axis
        
        uint8_t percent = Driver::RCPWM::get_pin_percent(i);
        // Count amout of shim changed from min. I.e. max = 2000, min = 1500 and 30% is used now
        // Then 500 * 0.3 = 150, so then we estimate angle as angle_per_shim_val * 150
        float current_shim_delta = static_cast<float>(percent)/100 * static_cast<float>(ch.max - ch.min);

        float deg_per_us = servo_coefficient_100us / 100.0f;
        // Multiply current change in shim on angle per shim coefficient
        float angle_deg = current_shim_delta * deg_per_us;
        // DEBUG VALS TO SEE WHAT'S GOING ON
        gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = percent;
        gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = angle_deg;
        gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = current_shim_delta;
    }
    
    // Convert RPY to Quaternion
    // STUB for now: Just logging the angles to verify

    char logMsg[42];
    sprintf(logMsg, "Gimbal Status: R=%.2f P=%.2f Y=%.2f", roll_deg, pitch_deg, yaw_deg);
    logger.log_debug(logMsg);
    
    gimbal_status_pub.publish();
}


void DronecanPwmFrontend::ahrs_solution_callback(
    const uavcan_equipment_ahrs_Solution& msg) {
    logger.log_debug("AHRS Solution received");
    if (msg.angular_velocity[0] > 0){
        return;
    }
    // msg.orientation_xyzw = current aircraft attitude
    // msg.angular_velocity = aircraft rotation rates
    // msg.linear_acceleration = aircraft acceleration
    
    // Store aircraft attitude for gimbal stabilization
    // backend->aircraft_attitude = msg.orientation_xyzw;
}

void DronecanPwmFrontend::arming_status_callback(const uavcan_equipment_safety_ArmingStatus& msg) {
    (void)msg;  // Not supported yet, what to do with SafetyArmingStatus?
}
