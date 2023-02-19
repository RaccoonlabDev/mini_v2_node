/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

/**
 * @file application.cpp
 * @author d.ponomarev
 * @date Jul 11, 2022
 */

#include "application.hpp"
#include "cyphal.hpp"
#include "cyphal_registers.hpp"
#include "main.h"
#include "string_params.hpp"
#include "storage.h"

void application_entry_point() {
    paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS);
    paramsLoadFromFlash();

    Cyphal cyphal;
    int res = cyphal.init();

    
    HAL_GPIO_WritePin(INTERNAL_LED_BLUE_GPIO_Port, INTERNAL_LED_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INTERNAL_LED_GREEN_GPIO_Port, INTERNAL_LED_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INTERNAL_LED_RED_GPIO_Port, INTERNAL_LED_RED_Pin, GPIO_PIN_SET);

    while (true) {
        GPIO_PinState state;

        if (HAL_GetTick() % 1000 > 500) {
            state = GPIO_PIN_SET;
        } else {
            state = GPIO_PIN_RESET;
        }
    
        if (res >= 0) {
            HAL_GPIO_WritePin(INTERNAL_LED_BLUE_GPIO_Port, INTERNAL_LED_BLUE_Pin, state);
        } else {
            HAL_GPIO_WritePin(INTERNAL_LED_RED_GPIO_Port, INTERNAL_LED_RED_Pin, state);
        }

        cyphal.process();
    }
}
