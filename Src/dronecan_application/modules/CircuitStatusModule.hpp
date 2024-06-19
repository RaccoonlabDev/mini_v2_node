/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#ifndef SRC_MODULES_CIRCUIT_STATUS_HPP_
#define SRC_MODULES_CIRCUIT_STATUS_HPP_

#include "dronecan/sensors/hygrometer/Hygrometer.h"
#include "logger.hpp"
#include "periphery/adc/circuit_periphery.hpp"
#include "periphery/led/led.hpp"
#include "periphery/sht3x/sht3x.hpp"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/temperature/Temperature.h"

class CircuitStatusModule {
  public:
    static CircuitStatusModule &get_instance();
    void spin_once();

    static bool instance_initialized;

  private:
    static CircuitStatusModule instance;

    CircuitStatus_t circuit_status = {};
    Temperature_t temperature_status = {};
    Hygrometer hygrometer_status = {};

    static Logger logger;

    CircuitStatusModule() {}

    uint8_t circuit_status_transfer_id = 0;
    uint8_t temperature_transfer_id = 0;
    uint8_t hygrometer_transfer_id = 0;

    int8_t init();
    CircuitStatusModule(const CircuitStatusModule &other) = delete;
    void operator=(const CircuitStatusModule &) = delete;
};

#endif // SRC_MODULES_CIRCUIT_STATUS_HPP_
