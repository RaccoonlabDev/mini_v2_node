/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
#define SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_

class CyphalPwmFrontend {
public:
    CyphalPwmFrontend() = default;
    void init();
    void update_params();
    void publish_gimbal_status();
};

#endif  // SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
