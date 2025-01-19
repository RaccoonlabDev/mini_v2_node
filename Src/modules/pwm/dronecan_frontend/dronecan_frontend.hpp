/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_
#define SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_

#include "dronecan.h"
#include "publisher.hpp"
#include "subscriber.hpp"
#include "modules/pwm/main.hpp"

enum class CommandType: uint8_t {
    RAW_COMMAND,
    ARRAY_COMMAND,
    HARDPOINT_COMMAND,

    NUMBER_OF_COMMANDS,
};

class DronecanPwmFrontend {
public:
    DronecanPwmFrontend() = default;

    void init(PWMModule* backend_);

    void update_params();

    static inline CommandType pwm_cmd_type{CommandType::RAW_COMMAND};

    static void raw_command_callback(const RawCommand_t& msg);
    static inline DronecanSubscriber<RawCommand_t> raw_command_sub;

    static void array_command_callback(const ArrayCommand_t& msg);
    static inline DronecanSubscriber<ArrayCommand_t> array_command_sub;

    static void hardpoint_callback(const HardpointCommand& msg);
    static inline DronecanSubscriber<HardpointCommand> hardpoint_sub;

    static void arming_status_callback(const SafetyArmingStatus& msg);
    static inline DronecanSubscriber<SafetyArmingStatus> arming_status_sub;

private:
    PWMModule* backend;
    static inline Logging logger{"DPWM"};
};

#endif  // SRC_MODULES_PWM_DRONECAN_FRONTEND_HPP_
