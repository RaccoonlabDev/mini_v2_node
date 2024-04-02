/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef SRC_MODULES_CIRCUIT_STATUS_HPP_
#define SRC_MODULES_CIRCUIT_STATUS_HPP_

#include "periphery/led/led.hpp"
#include "uavcan/equipment/temperature/Temperature.h"
#include "periphery/adc/circuit_periphery.hpp"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "logger.hpp"


class CircuitStatusModule {
public:
    static CircuitStatusModule &get_instance();
    void spin_once();

    static bool instance_initialized;
private:
    static CircuitStatusModule instance;

    CircuitStatus_t circuit_status = {};
    Temperature_t temperature_status = {};

    static Logger logger;

    CircuitStatusModule(){}

    uint8_t circuit_status_transfer_id  = 0;
    uint8_t temperature_transfer_id     = 0;

    int8_t init();
    CircuitStatusModule(const CircuitStatusModule &other) = delete;
    void operator=(const CircuitStatusModule &) = delete;
};

#endif  // SRC_MODULES_CIRCUIT_STATUS_HPP_
