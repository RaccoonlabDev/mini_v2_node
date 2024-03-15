/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine96@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#include "CircuitStatusModule.hpp"


CircuitStatusModule CircuitStatusModule::instance = CircuitStatusModule();
bool CircuitStatusModule::instance_initialized = false;
Logger CircuitStatusModule::logger = Logger("CircuitStatus");

CircuitStatusModule& CircuitStatusModule::get_instance() {
    if (!instance_initialized) {
        instance_initialized=true;
        instance.init();
    }
    return instance;
}

void CircuitStatusModule::init() {
    int8_t adc_status = adc.init();
    if (adc_status != 0) {
        logger.log_error("ADC init");
    } else {
        temp = adc.stm32Temperature(adc.get(AdcChannel::ADC_TEMPERATURE));
        vol_raw = adc.get(AdcChannel::ADC_VIN);
        cur_raw = adc.get(AdcChannel::ADC_CURRENT);
    }
}

void CircuitStatusModule::spin_once() {
    static uint32_t next_temp_pub_ms = 1000;
    static uint32_t next_status_pub_ms = 1000;

    if (v5_f > 5.5 || circuit_status.voltage > 60.0) {
        circuit_status.error_flags = ERROR_FLAG_OVERVOLTAGE;
    } else if (circuit_status.current > 1.05) {
        circuit_status.error_flags = ERROR_FLAG_OVERCURRENT;
    } else if (publish_error) {
        logger.log_debug("pub");
    }

    if (HAL_GetTick() > next_temp_pub_ms) {
        temp = adc.stm32Temperature(adc.get(AdcChannel::ADC_TEMPERATURE));
        temperature_status.temperature = temp;
        
        publish_error = dronecan_equipment_temperature_publish(&temperature_status, &temperature_transfer_id);
        temperature_transfer_id ++;
        next_temp_pub_ms += 10;
    }
    
    if (HAL_GetTick() > next_status_pub_ms) {
        v5_raw = adc.get(AdcChannel::ADC_5V);
        v5_f =  AdcPeriphery::stm32Voltage5V(v5_raw);

        vol_raw = adc.get(AdcChannel::ADC_VIN);
        cur_raw = adc.get(AdcChannel::ADC_CURRENT);

        circuit_status.voltage = AdcPeriphery::stm32Voltage(vol_raw);
        circuit_status.current = AdcPeriphery::stm32Current(cur_raw);

        publish_error = dronecan_equipment_circuit_status_publish(&circuit_status, &circuit_status_transfer_id);
        circuit_status_transfer_id ++;
        next_status_pub_ms += 10;

    }
    
    circuit_status.error_flags = ERROR_FLAG_CLEAR;
}
