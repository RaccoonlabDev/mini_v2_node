/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/rcout/router.hpp"
#include <cmath>

PwmRouter pwm_router;

int8_t PwmRouter::find_channel_idx(int16_t actuator_id) const {
    for (uint8_t idx = 0; idx < Driver::RCPWM::get_pins_amount(); idx++) {
        if (Driver::RCPWM::channels[idx].channel == actuator_id) {
            return idx;
        }
    }

    return -1;
}

void PwmRouter::apply(const ActuatorCommand& cmd) {
    auto channel_idx = find_channel_idx(cmd.actuator_id);
    if (channel_idx < 0) {
        return;
    }

    auto& rcout = Driver::RCPWM::channels[channel_idx];
    auto& timing = RcoutModule::timings[channel_idx];

    switch (cmd.kind) {
    case CommandKind::SIGNED_INT14:
        if (cmd.force_default) {
            timing.set_standby_state();
            rcout.set_default();
            return;
        }

        if (std::fabs(cmd.value) < 0.5f) {
            timing.set_default_state();
            rcout.set_percent(0);
            return;
        }

        if (cmd.engage_forever) {
            timing.set_engage_forever();
        } else {
            timing.set_engaged_state();
        }
        rcout.set_int14(static_cast<uint16_t>(cmd.value));
        return;

    case CommandKind::NORMALIZED_SIGNED:
        if (cmd.value < -0.001f || cmd.value > 0.001f) {
            if (cmd.engage_forever) {
                timing.set_engage_forever();
            } else {
                timing.set_engaged_state();
            }
        } else {
            timing.set_default_state();
        }
        rcout.set_normalized_signed(cmd.value);
        return;

    case CommandKind::NORMALIZED_UNSIGNED:
        if (cmd.value > 0.0f) {
            if (cmd.engage_forever) {
                timing.set_engage_forever();
            } else {
                timing.set_engaged_state();
            }
        } else {
            timing.set_default_state();
        }
        rcout.set_normalized_unsigned(cmd.value);
        return;

    case CommandKind::BOOL:
        if (cmd.engage_forever) {
            timing.set_engage_forever();
        } else if (cmd.value > 0.5f) {
            timing.set_engaged_state();
        } else {
            timing.set_default_state();
        }
        rcout.set_percent(cmd.value > 0.5f ? 100 : 0);
        return;

    case CommandKind::PERCENT:
        if (cmd.value > 0.0f) {
            if (cmd.engage_forever) {
                timing.set_engage_forever();
            } else {
                timing.set_engaged_state();
            }
        } else {
            timing.set_default_state();
        }
        rcout.set_percent(static_cast<int8_t>(cmd.value));
        return;

    default:
        return;
    }
}
