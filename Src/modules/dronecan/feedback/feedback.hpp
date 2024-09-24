/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_MODULES_FEEDBACK_HPP_
#define SRC_MODULES_FEEDBACK_HPP_

#include "common/module.hpp"
#include "peripheral/pwm/pwm.hpp"

#include "dronecan.h"
#include "logger.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"

#include "modules/dronecan/pwm/PWMModule.hpp"

enum class FeedbackType: uint8_t {
    DISABLED,
    DEFAULT_1_HZ,
    DEFAULT_10_HZ,

    NUMBER_OF_FEEDBACKS,
};

class FeedbackModule : public Module {
public:
    void init() override;

    inline FeedbackModule() : Module(10, Protocol::DRONECAN) {}

protected:
    void spin_once() override;
    void update_params() override;

    void publish_esc_status(PwmChannelInfo& pwm);
    void publish_actuator_status(PwmChannelInfo& pwm);
    void publish_hardpoint_status(PwmChannelInfo& pwm);
    
    static inline DronecanPublisher<ActuatorStatus_t> actuator_status;
    static inline DronecanPublisher<EscStatus_t> esc_status;
    static inline DronecanPublisher<HardpointStatus> hardpoint_status;

    CommandType cmd_type;
    FeedbackType feedback_type;
};

#endif  // SRC_MODULES_FEEDBACK_HPP_
