/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_ROUTER_HPP_
#define SRC_MODULES_PWM_ROUTER_HPP_

#include <cstdint>
#include "modules/rcout/main.hpp"

enum class CommandKind: uint8_t {
    BOOL,
    PERCENT,
    SIGNED_INT14,
    NORMALIZED_SIGNED,
    NORMALIZED_UNSIGNED,
};

struct ActuatorCommand {
    int16_t actuator_id{-1};                // Negative values mean disabled
    CommandKind kind{CommandKind::BOOL};    // How to interpret value (scaling/semantics)
    float value{0.0f};                      // Command payload
    bool force_default{false};              // Explicitly request default/standby
    bool engage_forever{false};             // Disable TTL for this command (persistent state)
};

class PwmRouter {
public:
    void apply(const ActuatorCommand& cmd);
private:
    int8_t find_channel_idx(int16_t actuator_id) const;
};

extern PwmRouter pwm_router;

#endif  // SRC_MODULES_PWM_ROUTER_HPP_
