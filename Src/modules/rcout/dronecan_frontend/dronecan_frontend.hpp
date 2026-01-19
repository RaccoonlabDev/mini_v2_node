/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_
#define SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_

#include "libdcnode/dronecan.h"
#include "libdcnode/pub.hpp"
#include "libdcnode/sub.hpp"
#include "common/logging.hpp"

enum class CommandType: uint8_t {
    RAW_COMMAND,
    ARRAY_COMMAND,
    HARDPOINT_COMMAND,

    NUMBER_OF_COMMANDS,
};

void set_gimbal_state_rpy(const float angles_rpy[3], uint16_t max_servos_angle);
    
void set_gimbal_state(const float q[4], uint16_t max_servos_angle);


class DronecanPwmFrontend {
public:
    DronecanPwmFrontend() = default;

    void init();

    void update_params();
    void publish_gimbal_status(); 
    
    static inline uint16_t max_servos_angle{90};

    // Setter and getter to handle static variable
    static void set_max_servos_angle(uint16_t angle) {
        max_servos_angle = angle;
    }
    static uint16_t get_max_servos_angle() {
        return max_servos_angle;
    }

    static inline CommandType pwm_cmd_type{CommandType::RAW_COMMAND};

    static void raw_command_callback(const uavcan_equipment_esc_RawCommand& msg);
    static inline libdcnode::DronecanSub<uavcan_equipment_esc_RawCommand> raw_command_sub;

    static void array_command_callback(const uavcan_equipment_actuator_ArrayCommand& msg);
    static inline libdcnode::DronecanSub<uavcan_equipment_actuator_ArrayCommand> array_command_sub;

    static void hardpoint_callback(const uavcan_equipment_hardpoint_Command& msg);
    static inline libdcnode::DronecanSub<uavcan_equipment_hardpoint_Command> hardpoint_sub;

    static void arming_status_callback(const uavcan_equipment_safety_ArmingStatus& msg);
    static inline libdcnode::DronecanSub<uavcan_equipment_safety_ArmingStatus> arming_status_sub;

    static void gimbal_angular_command_callback(const uavcan_equipment_camera_gimbal_AngularCommand& msg);
    static inline libdcnode::DronecanSub<uavcan_equipment_camera_gimbal_AngularCommand> gimbal_angular_command_sub;

    static inline libdcnode::DronecanPub<uavcan_equipment_camera_gimbal_Status> gimbal_status_pub;

private:
    static inline Logging logger{"DPWM"};
};

#endif  // SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_