/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include <array>
#include <span>
#include "drivers/rcpwm/rcpwm.hpp"
#include "params.hpp"

#define CH(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF

namespace {

std::array<Driver::RcpwmChannel, 4> CHANNELS = {{
    Driver::RcpwmChannel(MIN(1), MAX(1), DEF(1), CH(1), 0),
    Driver::RcpwmChannel(MIN(2), MAX(2), DEF(2), CH(2), 1),
    Driver::RcpwmChannel(MIN(3), MAX(3), DEF(3), CH(3), 2),
    Driver::RcpwmChannel(MIN(4), MAX(4), DEF(4), CH(4), 3),
}};

}  // namespace

std::span<Driver::RcpwmChannel> Driver::RCPWM::channels{CHANNELS};
