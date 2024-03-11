/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/pwm/pwm.hpp"
#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

struct PwmPinInfo {
    TIM_HandleTypeDef& htim;
    uint32_t channel;
    volatile uint32_t& ccr;
};

const static PwmPinInfo info[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.htim = htim4,     .channel = TIM_CHANNEL_2,   .ccr = TIM4->CCR2},     // PB7
    {.htim = htim4,     .channel = TIM_CHANNEL_1,   .ccr = TIM4->CCR1},     // PB6
    {.htim = htim3,     .channel = TIM_CHANNEL_1,   .ccr = TIM3->CCR1},     // PB4
    {.htim = htim3,     .channel = TIM_CHANNEL_2,   .ccr = TIM3->CCR2},     // PB5
};

int8_t PwmPeriphery::init(PwmPin pwm_pin) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return -1;
    }

    auto& pwm_pin_info = info[static_cast<uint8_t>(pwm_pin)];
    return HAL_OK == HAL_TIM_PWM_Start(&pwm_pin_info.htim, pwm_pin_info.channel) ? 0 : -1;
}

void PwmPeriphery::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return;
    }

    auto& pwm_pin_info = info[static_cast<uint8_t>(pwm_pin)];
    pwm_pin_info.ccr = duration_us;
}

uint32_t PwmPeriphery::get_duration(PwmPin pwm_pin) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return 0;
    }

    auto& pwm_pin_info = info[static_cast<uint8_t>(pwm_pin)];
    return pwm_pin_info.ccr;
}
