/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
#define SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_

#include "uavcan/equipment/temperature/Temperature.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "common/module.hpp"

class CircuitStatus : public Module {
public:
    CircuitStatus() : Module(2) {}
    void init() override;

protected:
    void spin_once() override;

private:
    CircuitStatus_t circuit_status = {};
    Temperature_t temperature_status = {};

    uint8_t circuit_status_transfer_id  = 0;
    uint8_t temperature_transfer_id     = 0;
};

#endif  // SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
