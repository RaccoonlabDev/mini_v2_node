/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "main.h"

void uavcanReadUniqueID(uint8_t out_uid[4]) {
    const uint32_t UNIQUE_ID_16_BYTES[4] = {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2(),
        0
    };
    memcpy(out_uid, UNIQUE_ID_16_BYTES, 16);
}

void uavcanRestartNode() {
    HAL_NVIC_SystemReset();
}

uint32_t uavcanGetTimeMs() {
    return HAL_GetTick();
}
