/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
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
    // Current feedback
    uint16_t new_max_servos_travel = 90;  // degrees
    uint16_t cached_max_servos_travel = 90;
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
