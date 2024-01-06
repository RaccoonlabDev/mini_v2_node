/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "params.hpp"
#include "string_params.hpp"
#include "storage.h"

IntegerDesc_t integer_desc_pool[] = {
    {"uavcan.node.id", 0, 127, 50, MUTABLE},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
    {"system.name", "co.raccoonlab.mini", IMMUTABLE},
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
