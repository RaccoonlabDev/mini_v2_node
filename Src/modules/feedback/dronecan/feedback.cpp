/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "feedback.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "peripheral/adc/circuit_periphery.hpp"
#include "drivers/rcpwm/rcpwm.hpp"
#include "modules/rcout/dronecan_frontend/dronecan_frontend.hpp"
#include "params.hpp"
#include "logging.hpp"

REGISTER_MODULE(DronecanFeedbackModule)


void DronecanFeedbackModule::init() {
    set_mode(Mode::STANDBY);
}

void DronecanFeedbackModule::update_params() {
    feedback_esc_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_ESC_ENABLE));
    feedback_actuator_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_ACTUATOR_ENABLE));
    feedback_hardpoint_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_HARDPOINT_ENABLE));
    feedback_gimbal_enabled = static_cast<Gimbal_enable>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_GIMBAL_ENABLE));
}

void DronecanFeedbackModule::spin_once() {
    for (uint_fast8_t pin_idx = 0; pin_idx < Driver::RCPWM::get_pins_amount(); pin_idx++) {
        if (!Driver::RCPWM::is_pin_enabled(pin_idx)) {
            continue;
        }

        if (feedback_esc_enabled) {
            publish_esc_status(pin_idx);
        }

        if (feedback_actuator_enabled) {
            publish_actuator_status(pin_idx);
        }

        if (feedback_hardpoint_enabled) {
            publish_hardpoint_status(pin_idx);
        }
    }

    if (feedback_gimbal_enabled == Gimbal_enable::ENABLE_DEG){
        publish_gimbal_status_rpy();
    }
    if (feedback_gimbal_enabled == Gimbal_enable::ENABLE_QUAT){
        publish_gimbal_status_quaternion();
    }
}

void DronecanFeedbackModule::publish_esc_status(uint8_t pin_idx) {
    esc_status.msg = {
        .error_count = esc_status.msg.error_count + 1, // FIXME: implement proper error counting
        .voltage = CircuitPeriphery::voltage_vin(),
        .current = CircuitPeriphery::current(),
        .temperature = static_cast<float>(CircuitPeriphery::temperature()),
        .rpm = 0,
        .power_rating_pct = Driver::RCPWM::get_pin_percent(pin_idx),
        .esc_index = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),
    };

    esc_status.publish();
}

void DronecanFeedbackModule::publish_actuator_status(uint8_t pin_idx) {
    actuator_status.msg = {
        .actuator_id = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),

        // The following fields are not used in PX4 anyway
        // Let's fill them with something useful for logging for a while
        .position = CircuitPeriphery::voltage_5v(),
        .force = CircuitPeriphery::current(),
        .speed = static_cast<float>(CircuitPeriphery::temperature()),

        .power_rating_pct = Driver::RCPWM::get_pin_percent(pin_idx),
    };

    actuator_status.publish();
}

void DronecanFeedbackModule::publish_hardpoint_status(uint8_t pin_idx) {
    static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
    static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

    hardpoint_status.msg = {
        .hardpoint_id = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),
        .payload_weight = 0.0f,
        .payload_weight_variance = 0.0f,
        .status = Driver::RCPWM::get_pin_percent(pin_idx) == 0 ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX,
    };

    // Hardpoint serialization has a bug, let's skip it until it is fixed
    // hardpoint_status.publish();
}

void DronecanFeedbackModule::publish_gimbal_status_rpy() {

    gimbal_status_pub.msg.gimbal_id = 0; //
    gimbal_status_pub.msg.mode.command_mode = 1;
    
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = 0; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = 0; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = 0; // Z
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[3] = 1; // W
    
    float roll_deg = 0, pitch_deg = 0, yaw_deg = 0;
    for (size_t i = 0; i < Driver::RCPWM::get_pins_amount(); ++i) {
        switch (Driver::RCPWM::get_pin_channel(i)) {
            case 0: // Roll
                roll_deg = static_cast<float>(Driver::RCPWM::get_current_angle(gimbal::get_max_servos_angle(), i));
                break;
            case 1: // Pitch
                pitch_deg = static_cast<float>(Driver::RCPWM::get_current_angle(gimbal::get_max_servos_angle(), i));
                break;
            case 2: // Yaw
                yaw_deg = static_cast<float>(Driver::RCPWM::get_current_angle(gimbal::get_max_servos_angle(), i));
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

void DronecanFeedbackModule::publish_gimbal_status_quaternion() {

    gimbal_status_pub.msg.gimbal_id = 0;
    gimbal_status_pub.msg.mode.command_mode = 1;
    
    // Set default values for safety
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = 0; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = 0; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = 0; // Z
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[3] = 1; // W
    
    const float* q = gimbal::get_quaternion(); 

    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[0] = q[0]; // X
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[1] = q[1]; // Y
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[2] = q[2]; // Z
    gimbal_status_pub.msg.camera_orientation_in_body_frame_xyzw[3] = q[3]; // Z

    gimbal_status_pub.publish();
}
