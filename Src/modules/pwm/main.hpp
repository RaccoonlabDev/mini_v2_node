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
#include "ttl.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "common/algorithms.hpp"
#include "common/module.hpp"
#include "common/logging.hpp"
#include "drivers/rcpwm/rcpwm.hpp"


class PWMModule : public Module {
public:
    PWMModule() : Module(50, Protocol::CYPHAL_AND_DRONECAN) {}
    void init() override;

    static inline uint16_t cmd_ttl{500};
    static inline Logging logger{"PWM"};

    static inline std::array<SetpointTimings, Driver::RCPWM::get_pins_amount()> timings{};

protected:
    void update_params() override;
    void spin_once() override;
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
