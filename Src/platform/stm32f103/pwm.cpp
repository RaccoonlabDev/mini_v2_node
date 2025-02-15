/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/pwm/pwm.hpp"
#include "peripheral/pwm/pwm_stm32.hpp"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

namespace HAL {

const std::array<PwmPinInfo, static_cast<uint8_t>(PwmPin::PWM_AMOUNT)> pwms = {{
    {.htim = htim4,     .channel = TIM_CHANNEL_2,   .ccr = TIM4->CCR2},     // PB7
    {.htim = htim4,     .channel = TIM_CHANNEL_1,   .ccr = TIM4->CCR1},     // PB6
    {.htim = htim3,     .channel = TIM_CHANNEL_1,   .ccr = TIM3->CCR1},     // PB4
    {.htim = htim3,     .channel = TIM_CHANNEL_2,   .ccr = TIM3->CCR2},     // PB5
}};

}  // namespace HAL
