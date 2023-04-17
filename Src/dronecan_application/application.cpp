/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "application.hpp"
#include "uavcan.h"
#include "uavcan_application.h"
#include "main.h"
#include "params.hpp"
#include "string_params.hpp"
#include "storage.h"

void application_entry_point() {
    paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS);
    paramsLoadFromFlash();

    HAL_GPIO_WritePin(INTERNAL_LED_BLUE_GPIO_Port, INTERNAL_LED_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INTERNAL_LED_GREEN_GPIO_Port, INTERNAL_LED_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INTERNAL_LED_RED_GPIO_Port, INTERNAL_LED_RED_Pin, GPIO_PIN_SET);

    uavcanInitApplication(42);

    while(true) {
        GPIO_PinState state = (HAL_GetTick() % 1000 > 500) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(INTERNAL_LED_BLUE_GPIO_Port, INTERNAL_LED_BLUE_Pin, state);

        canardSpinNodeStatus();
        uavcanProcessSending();
        uavcanProcessReceiving();
    }
}
