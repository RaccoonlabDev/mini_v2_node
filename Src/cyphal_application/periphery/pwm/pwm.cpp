/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022-2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "pwm.hpp"
#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

int8_t PwmPeriphery::init(PwmPin pwm_pin) {
    switch (pwm_pin) {
        case PwmPin::PWM_1:
            if (HAL_OK != HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2)) {
                return -1;
            }
            TIM4->CCR2 = 1000;
            break;

        case PwmPin::PWM_2:
            if (HAL_OK != HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1)) {
                return -1;
            }
            TIM4->CCR1 = 1000;
            break;

        case PwmPin::PWM_3:
            if (HAL_OK != HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1)) {
                return -1;
            }
            TIM3->CCR1 = 1000;
            break;

        case PwmPin::PWM_4:
            if (HAL_OK != HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2)) {
                return -1;
            }
            TIM3->CCR2 = 1000;
            break;

        default:
            return -1;
    }

    return 0;
}

void PwmPeriphery::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
    switch (pwm_pin) {
        case PwmPin::PWM_1:
            TIM4->CCR2 = duration_us;
            break;

        case PwmPin::PWM_2:
            TIM4->CCR1 = duration_us;
            break;

        case PwmPin::PWM_3:
            TIM3->CCR1 = duration_us;
            break;

        case PwmPin::PWM_4:
            TIM3->CCR2 = duration_us;
            break;

        default:
            break;
    }
}

uint32_t PwmPeriphery::get_duration(PwmPin pwm_pin) {
    uint32_t pwm_duration;

    switch (pwm_pin) {
        case PwmPin::PWM_1:
            pwm_duration = TIM4->CCR2;
            break;

        case PwmPin::PWM_2:
            pwm_duration = TIM4->CCR1;
            break;

        case PwmPin::PWM_3:
            pwm_duration = TIM3->CCR1;
            break;

        case PwmPin::PWM_4:
            pwm_duration = TIM3->CCR2;
            break;

        default:
            pwm_duration = 0;
            break;
    }

    return pwm_duration;
}
