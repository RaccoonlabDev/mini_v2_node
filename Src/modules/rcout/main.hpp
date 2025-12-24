/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_PWMMODULE_HPP_
#define SRC_MODULES_PWM_PWMMODULE_HPP_

#include <algorithm>
#include "params.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "common/algorithms.hpp"
#include "common/module.hpp"
#include "common/logging.hpp"
#include "drivers/rcpwm/rcpwm.hpp"
#include "modules/rcout/ttl.hpp"


class RcoutModule : public Module {
public:
    RcoutModule() : Module(50, Protocol::CYPHAL_AND_DRONECAN) {}
    void init() override;

    static inline uint16_t cmd_ttl{500};
    static inline Logging logger{"RCOUT"};

    static inline std::array<ActuatorTtl, Driver::RCPWM::get_pins_amount()> timings{};

protected:
    void update_params() override;
    void spin_once() override;
private:
    // Gimbal state (cached from callbacks)
    std::array<float, 4> gimbal_target_attitude{0, 0, 0, 1};  // xyzw quaternion
    uint8_t gimbal_command_mode{0};  // COMMAND_MODE_ORIENTATION_BODY_FRAME, etc.
    bool gimbal_is_active{false};
    // Current feedback
    std::array<float, 4> gimbal_current_attitude{0, 0, 0, 1};  // xyzw
    std::array<float, 9> gimbal_covariance{};  // 3x3 matrix for orientation uncertainty
    uint8_t servo_coefficient = 1;
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
