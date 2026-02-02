/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#ifndef SRC_MODULES_FEEDBACK_HPP_
#define SRC_MODULES_FEEDBACK_HPP_

#include "common/module.hpp"

#include "libdcnode/dronecan.h"
#include "libdcnode/logger.hpp"
#include "libdcnode/pub.hpp"
#include "libdcnode/sub.hpp"

class DronecanFeedbackModule : public Module {
public:
    void init() override;

    inline DronecanFeedbackModule() : Module(10, Protocol::DRONECAN) {}

    enum class Gimbal_enable : uint8_t {
        DISABLED                    = 0,
        ENABLE_DEG                  = 1,
        ENABLE_QUAT                 = 2
    };

protected:
    void spin_once() override;
    void update_params() override;

private:
    void publish_esc_status(uint8_t pin_idx);
    void publish_actuator_status(uint8_t pin_idx);
    void publish_hardpoint_status(uint8_t pin_idx);
    void publish_gimbal_status_rpy();
    void publish_gimbal_status_quaternion();

    static inline libdcnode::DronecanPub<uavcan_equipment_camera_gimbal_Status> gimbal_status_pub;
    static inline libdcnode::DronecanPub<uavcan_equipment_actuator_Status> actuator_status;
    static inline libdcnode::DronecanPub<uavcan_equipment_esc_Status> esc_status;
    static inline libdcnode::DronecanPub<uavcan_equipment_hardpoint_Status> hardpoint_status;

    bool feedback_esc_enabled{false};
    bool feedback_actuator_enabled{false};
    bool feedback_hardpoint_enabled{false};
    Gimbal_enable feedback_gimbal_enabled{0};
};

#endif  // SRC_MODULES_FEEDBACK_HPP_
