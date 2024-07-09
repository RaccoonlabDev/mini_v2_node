/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#include "CircuitStatusModule.hpp"
#include "periphery/adc/circuit_periphery.hpp"

void CircuitStatus::init() {
    mode = Module::Mode::OPEARTIONAL;
}

void CircuitStatus::spin_once() {
    temperature_status.temperature = CircuitPeriphery::temperature();
    dronecan_equipment_temperature_publish(&temperature_status, &temperature_transfer_id);
    temperature_transfer_id++;

    circuit_status.voltage = CircuitPeriphery::voltage_vin();
    circuit_status.current = CircuitPeriphery::current();
    circuit_status.error_flags = ERROR_FLAG_CLEAR;
    if (CircuitPeriphery::voltage_5v() > 5.5 || circuit_status.voltage > 60.0) {
        circuit_status.error_flags = ERROR_FLAG_OVERVOLTAGE;
    } else if (circuit_status.current > 1.05) {
        circuit_status.error_flags = ERROR_FLAG_OVERCURRENT;
    }
    dronecan_equipment_circuit_status_publish(&circuit_status, &circuit_status_transfer_id);
    circuit_status_transfer_id++;

    if (circuit_status.error_flags != ERROR_FLAG_CLEAR) {
        health = Status::MINOR_FAILURE;
    }
}
