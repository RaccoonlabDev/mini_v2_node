/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "circuit_status.hpp"
#include <algorithm>
#include "cyphal.hpp"
#include "params.hpp"

#include "uavcan/si/sample/electric_current/Scalar_1_0.h"
#include "uavcan/si/sample/voltage/Scalar_1_0.h"
#include "uavcan/si/sample/temperature/Scalar_1_0.h"


int8_t CircuitStatus::init() {
    return CircuitPeriphery::init();
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
        float volt = CircuitPeriphery::voltage_5v();
        voltage_5v_pub.publish(uavcan_si_sample_voltage_Scalar_1_0{0, volt});
    }

    if (voltage_vin_pub.isEnabled()) {
        float volt = CircuitPeriphery::voltage_vin();
        voltage_vin_pub.publish(uavcan_si_sample_voltage_Scalar_1_0{0, volt});
    }

    if (temperature_pub.isEnabled()) {
        auto temp = (float)CircuitPeriphery::temperature();
        temperature_pub.publish(uavcan_si_sample_temperature_Scalar_1_0{0, temp});
    }
}
