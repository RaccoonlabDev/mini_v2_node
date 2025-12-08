/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "drivers/rcpwm/rcpwm.hpp"
#include "params.hpp"

#define CH(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF

std::array<Driver::RcpwmChannel, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)>
    Driver::RCPWM::channels = {{
        RcpwmChannel(MIN(1), MAX(1), DEF(1), CH(1), HAL::PwmPin::PWM_1),
        RcpwmChannel(MIN(2), MAX(2), DEF(2), CH(2), HAL::PwmPin::PWM_2),
        RcpwmChannel(MIN(3), MAX(3), DEF(3), CH(3), HAL::PwmPin::PWM_3),
        RcpwmChannel(MIN(4), MAX(4), DEF(4), CH(4), HAL::PwmPin::PWM_4),
    }};
