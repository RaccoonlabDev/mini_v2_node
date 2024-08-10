/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#include "CircuitStatusModule.hpp"
#include "periphery/adc/circuit_periphery.hpp"

static CircuitStatus circuit_status_module;

void CircuitStatus::init() {
    mode = Module::Mode::OPERATIONAL;
}

void CircuitStatus::spin_once() {
    dev_temperature_pub.msg.temperature = CircuitPeriphery::temperature();
    dev_temperature_pub.publish();

    auto& circuit_status = circuit_status_pub.msg;
    circuit_status.voltage = CircuitPeriphery::voltage_vin();
    circuit_status.current = CircuitPeriphery::current();
    circuit_status.error_flags = ERROR_FLAG_CLEAR;
    if (CircuitPeriphery::voltage_5v() > 5.5 || circuit_status.voltage > 60.0) {
        circuit_status.error_flags = ERROR_FLAG_OVERVOLTAGE;
    } else if (circuit_status.current > 1.05) {
        circuit_status.error_flags = ERROR_FLAG_OVERCURRENT;
    }
    circuit_status_pub.publish();

    if (circuit_status.error_flags != ERROR_FLAG_CLEAR) {
        health = Status::MINOR_FAILURE;
    }
}
