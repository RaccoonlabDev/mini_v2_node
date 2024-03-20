/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#include "CircuitStatusModule.hpp"


CircuitStatusModule CircuitStatusModule::instance = CircuitStatusModule();
bool CircuitStatusModule::instance_initialized = false;
Logger CircuitStatusModule::logger = Logger("CircuitStatus");

CircuitStatusModule& CircuitStatusModule::get_instance() {
    if (!instance_initialized) {
        if (instance.init() != 0) {
            logger.log_debug("ADC init error");
        } else {
            instance_initialized = true;
        };
    }
    return instance;
}

int8_t CircuitStatusModule::init() {
    return CircuitPeriphery::init();
}

void CircuitStatusModule::spin_once() {
    static uint32_t next_temp_pub_ms = 1000;
    static uint32_t next_status_pub_ms = 1000;

    if (CircuitPeriphery::voltage_5v() > 5.5 || circuit_status.voltage > 60.0) {
        circuit_status.error_flags = ERROR_FLAG_OVERVOLTAGE;
    } else if (circuit_status.current > 1.05) {
        circuit_status.error_flags = ERROR_FLAG_OVERCURRENT;
    } else if (publish_error) {
        logger.log_debug("pub");
    }

    if (HAL_GetTick() > next_temp_pub_ms) {
        temperature_status.temperature = CircuitPeriphery::temperature();
        
        publish_error = dronecan_equipment_temperature_publish(&temperature_status, &temperature_transfer_id);
        temperature_transfer_id ++;
        next_temp_pub_ms += 1000;
    }
    
    if (HAL_GetTick() > next_status_pub_ms) {

        circuit_status.voltage = CircuitPeriphery::voltage_vin();
        circuit_status.current = CircuitPeriphery::current();

        publish_error = dronecan_equipment_circuit_status_publish(&circuit_status, &circuit_status_transfer_id);
        circuit_status_transfer_id ++;
        next_status_pub_ms += 1000;

    }
    
    circuit_status.error_flags = ERROR_FLAG_CLEAR;
}
