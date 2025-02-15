/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "pwm_stm32.hpp"
#include "common/algorithms.hpp"

extern TIM_HandleTypeDef htim4;

namespace HAL {

int8_t Pwm::init(PwmPin pwm_pin) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return -1;
    }

    auto& pwm = pwms[static_cast<uint8_t>(pwm_pin)];
    return HAL_OK == HAL_TIM_PWM_Start(&pwm.htim, pwm.channel) ? 0 : -1;
}

void Pwm::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return;
    }

    pwms[static_cast<uint8_t>(pwm_pin)].ccr = duration_us;
}

uint32_t Pwm::get_duration(PwmPin pwm_pin) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return 0;
    }

    return pwms[static_cast<uint8_t>(pwm_pin)].ccr;
}

uint8_t Pwm::get_percent(PwmPin pin, uint32_t min_duration_us, uint32_t max_duration_us) {
    return mapPwmToPct(get_duration(pin), min_duration_us, max_duration_us);
}

void Pwm::set_frequency(PwmPin pwm_pin, uint32_t frequency_hz) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return;
    }

    auto& pwm = pwms[static_cast<uint8_t>(pwm_pin)];
    volatile uint32_t* arr_reg = &(pwm.htim.Instance->ARR);
    uint16_t period_us = 1000000 / frequency_hz;
    *arr_reg = period_us;
}

uint32_t Pwm::get_frequency(PwmPin pwm_pin) {
    if (pwm_pin > PwmPin::PWM_AMOUNT) {
        return -1;
    }

    auto& pwm = pwms[static_cast<uint8_t>(pwm_pin)];
    volatile uint32_t* arr_reg = &(pwm.htim.Instance->ARR);
    uint32_t frequency = 1000000 / *arr_reg;
    return frequency;
}

}  // namespace HAL
