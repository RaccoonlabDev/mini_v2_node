/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/rcout/router.hpp"
#include <cmath>

PwmRouter pwm_router;

void PwmRouter::apply(const ActuatorCommand& cmd) {
    for (uint8_t channel_idx = 0; channel_idx < Driver::RCPWM::get_pins_amount(); channel_idx++) {
        if (Driver::RCPWM::channels[channel_idx].channel != cmd.actuator_id) {
            continue;
        }

        auto& rcout = Driver::RCPWM::channels[channel_idx];
        auto& timing = RcoutModule::timings[channel_idx];

        switch (cmd.kind) {
        case CommandKind::SIGNED_INT14:
            if (cmd.force_default) {
                timing.set_standby_state();
                rcout.set_default();
                continue;
            }

            if (std::fabs(cmd.value) < 0.5f) {
                timing.set_default_state();
                rcout.set_percent(0);
                continue;
            }

            if (cmd.engage_forever) {
                timing.set_engage_forever();
            } else {
                timing.set_engaged_state();
            }
            rcout.set_int14(static_cast<uint16_t>(cmd.value));
            continue;

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
            continue;

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
            continue;

        case CommandKind::BOOL:
            if (cmd.engage_forever) {
                timing.set_engage_forever();
            } else if (cmd.value > 0.5f) {
                timing.set_engaged_state();
            } else {
                timing.set_default_state();
            }
            rcout.set_percent(cmd.value > 0.5f ? 100 : 0);
            continue;

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
            continue;

        default:
            continue;
        }
    }
}
