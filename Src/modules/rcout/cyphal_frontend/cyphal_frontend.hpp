/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include <cstdint>

#ifndef SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
#define SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_

class CyphalPwmFrontend {
public:
    CyphalPwmFrontend() = default;
    void init();
    void update_params();

    static inline uint16_t max_servos_angle{90};

    static void set_max_servos_angle(uint16_t angle) {
        max_servos_angle = angle;
    }
    static uint16_t get_max_servos_angle() {
        return max_servos_angle;
    }
};

#endif  // SRC_MODULES_PWM_CYPHAL_FRONTEND_HPP_
