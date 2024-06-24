/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#include "CircuitStatusModule.hpp"


Logger CircuitStatus::logger = Logger("CircuitStatus");

void CircuitStatus::init() {
    CircuitPeriphery::init();
    mode = Module::Mode::OPEARTIONAL;
}

void CircuitStatus::spin_once() {
    static uint32_t next_temp_pub_ms = 1000;
    static uint32_t next_status_pub_ms = 1000;

    if (CircuitPeriphery::voltage_5v() > 5.5 || circuit_status.voltage > 60.0) {
        circuit_status.error_flags = ERROR_FLAG_OVERVOLTAGE;
    } else if (circuit_status.current > 1.05) {
        circuit_status.error_flags = ERROR_FLAG_OVERCURRENT;
    }

    if (HAL_GetTick() > next_temp_pub_ms) {
        temperature_status.temperature = CircuitPeriphery::temperature();

        dronecan_equipment_temperature_publish(&temperature_status, &temperature_transfer_id);
        temperature_transfer_id++;
        next_temp_pub_ms += 1000;
    }

    if (HAL_GetTick() > next_status_pub_ms) {
        circuit_status.voltage = CircuitPeriphery::voltage_vin();
        circuit_status.current = CircuitPeriphery::current();

        dronecan_equipment_circuit_status_publish(&circuit_status, &circuit_status_transfer_id);
        circuit_status_transfer_id++;
        next_status_pub_ms += 1000;
    }

    if (circuit_status.error_flags != ERROR_FLAG_CLEAR) {
        health = Status::MINOR_FAILURE;
    }

    circuit_status.error_flags = ERROR_FLAG_CLEAR;
}
