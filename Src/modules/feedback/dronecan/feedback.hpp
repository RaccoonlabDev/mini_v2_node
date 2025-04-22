/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_MODULES_FEEDBACK_HPP_
#define SRC_MODULES_FEEDBACK_HPP_

#include "common/module.hpp"

#include "dronecan.h"
#include "logger.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"

class DronecanFeedbackModule : public Module {
public:
    void init() override;

    inline DronecanFeedbackModule() : Module(10, Protocol::DRONECAN) {}

protected:
    void spin_once() override;
    void update_params() override;

private:
    void publish_esc_status(uint8_t pin_idx);
    void publish_actuator_status(uint8_t pin_idx);
    void publish_hardpoint_status(uint8_t pin_idx);
    
    static inline DronecanPublisher<ActuatorStatus_t> actuator_status;
    static inline DronecanPublisher<EscStatus_t> esc_status;
    static inline DronecanPublisher<HardpointStatus> hardpoint_status;

    bool feedback_esc_enabled{false};
    bool feedback_actuator_enabled{false};
    bool feedback_hardpoint_enabled{false};
};

#endif  // SRC_MODULES_FEEDBACK_HPP_
