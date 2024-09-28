/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
#define SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_

#include "modules/pwm/main.hpp"

class CyphalPwmFrontend {
public:
    CyphalPwmFrontend() = default;
    void init(PWMModule* backend_);

private:
    PWMModule* backend;
};

#endif  // SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
