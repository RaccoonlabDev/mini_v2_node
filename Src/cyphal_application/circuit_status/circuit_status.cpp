/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2024 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "circuit_status.hpp"
#include <algorithm>
#include "cyphal.hpp"
#include "params.hpp"

#include "uavcan/si/sample/electric_current/Scalar_1_0.h"
#include "uavcan/si/sample/voltage/Scalar_1_0.h"
#include "uavcan/si/sample/temperature/Scalar_1_0.h"

#define ADC_RAW_TO_5V  (3.3f * 2.0f / 4095.0f)
#define ADC_RAW_TO_VIN (3.3f * 19.0f / 4095.0f)

#ifdef STM32G0B1xx
    static const uint16_t TEMP_REF = 30;
    static const uint16_t ADC_REF = 943;    ///< v_ref / 3.3 * 4095
    static const uint16_t AVG_SLOPE = 3.1;  ///< avg_slope/(3.3/4096)
#else  // STM32F103xB
    static const uint16_t TEMP_REF = 25;
    static const uint16_t ADC_REF = 1750;   ///< v_ref / 3.3 * 4095
    static const uint16_t AVG_SLOPE = 5;    ///< avg_slope/(3.3/4096)
#endif

int8_t CircuitStatus::init() {
    periphery = CircuitPeriphery();
    return periphery.init();
}

void CircuitStatus::process(uint32_t crnt_time_ms) {
    if (_prev_pub_ts_ms + 500 > crnt_time_ms) {
        return;
    }
    _prev_pub_ts_ms = crnt_time_ms;

    _update_parameters();
    _spin_once();
}

void CircuitStatus::_update_parameters() {
    uint16_t port_id;

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_5V_ID));
    voltage_5v_pub.setPortId(port_id);

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_VIN_ID));
    voltage_vin_pub.setPortId(port_id);

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_TEMPERATURE_ID));
    temperature_pub.setPortId(port_id);
}

void CircuitStatus::_spin_once() {
    if (voltage_5v_pub.isEnabled()) {
        float volt = periphery.internal_volt_5v();
        voltage_5v_pub.publish(uavcan_si_sample_voltage_Scalar_1_0{0, volt});
    }

    if (voltage_vin_pub.isEnabled()) {
        float volt = periphery.internal_volt();
        voltage_vin_pub.publish(uavcan_si_sample_voltage_Scalar_1_0{0, volt});
    }

    if (temperature_pub.isEnabled()) {
        auto temp = (float)periphery.internal_temp();
        temperature_pub.publish(uavcan_si_sample_temperature_Scalar_1_0{0, temp});
    }
}
