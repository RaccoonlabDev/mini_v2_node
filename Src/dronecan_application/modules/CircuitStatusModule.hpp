/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine96@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef SRC_MODULE_CIRCUIT_STATUS_HPP_
#define SRC_MODULE_CIRCUIT_STATUS_HPP_

#include "periphery/led/led.hpp"
#include "uavcan/equipment/temperature/Temperature.h"
#include "periphery/adc/circuit_periphery.hpp"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "logger.hpp"


class CircuitStatusModule {
public:
    static CircuitStatusModule &get_instance();
    void spin_once();

    float temp = 0;
    float v5_f = 0.0;
    float volt = 0;
    float curr = 0;

    static bool instance_initialized;
private:
    static CircuitStatusModule instance;

    CircuitStatus_t circuit_status = {};
    Temperature_t temperature_status = {};
    
    static Logger logger;

    CircuitStatusModule(){};
    CircuitPeriphery circuit_periphery;

    uint8_t circuit_status_transfer_id  = 0;
    uint8_t temperature_transfer_id     = 0;

    uint16_t v5_raw     = 0;
    uint16_t cur_raw    = 0;
    uint16_t vol_raw    = 0;
    uint16_t tem_raw    = 0;

    int8_t publish_error = 0;

    int8_t init();
    CircuitStatusModule(CircuitStatusModule &other) = delete;
    void operator=(const CircuitStatusModule &) = delete;
};

#endif //SRC_MODULE_CIRCUIT_STATUS_HPP_
