/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "circuit_status.hpp"
#include <algorithm>
#include "libcpnode/cyphal.hpp"
#include "peripheral/adc/circuit_periphery.hpp"
#include "params.hpp"

REGISTER_MODULE(CyphalCircuitStatus)

void CyphalCircuitStatus::init() {
    set_mode(Mode::STANDBY);
    auto cyphal = libcpnode::Cyphal::get_instance();

    voltage_5v_pub = cyphal->makePublisher<uavcan_si_unit_voltage_Scalar_1_0>(
        static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_5V_ID))
    );
    voltage_vin_pub = cyphal->makePublisher<uavcan_si_unit_voltage_Scalar_1_0>(
        static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_VIN_ID))
    );
    temperature_pub = cyphal->makePublisher<uavcan_si_unit_temperature_Scalar_1_0>(
        static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_TEMPERATURE_ID))
    );
}

void CyphalCircuitStatus::update_params() {
    uint16_t port_id;

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_5V_ID));
    voltage_5v_pub.setPortId(port_id);

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_VIN_ID));
    voltage_vin_pub.setPortId(port_id);

    port_id = static_cast<uint16_t>(paramsGetIntegerValue(PARAM_PUB_CRCT_TEMPERATURE_ID));
    temperature_pub.setPortId(port_id);
}

void CyphalCircuitStatus::spin_once() {
    if (voltage_5v_pub.isEnabled()) {
        voltage_5v_pub.msg.volt = CircuitPeriphery::voltage_5v();
        voltage_5v_pub.publish();
    }

    if (voltage_vin_pub.isEnabled()) {
        voltage_vin_pub.msg.volt = CircuitPeriphery::voltage_vin();
        voltage_vin_pub.publish();
    }

    if (temperature_pub.isEnabled()) {
        temperature_pub.msg.kelvin = CircuitPeriphery::temperature();
        temperature_pub.publish();
    }

    set_health(CircuitPeriphery::is_failure() ? Status::MINOR_FAILURE : Status::OK);
}
