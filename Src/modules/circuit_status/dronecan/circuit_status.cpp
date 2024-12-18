/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "circuit_status.hpp"
#include "params.hpp"
#include "peripheral/adc/circuit_periphery.hpp"

REGISTER_MODULE(DronecanCircuitStatus)

void DronecanCircuitStatus::init() {
    set_mode(Mode::STANDBY);
}

void DronecanCircuitStatus::update_params() {
    node_id = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID));
    bitmask = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_CRCT_BITMASK));
}

void DronecanCircuitStatus::spin_once() {
    if (bitmask & static_cast<uint8_t>(Bitmask::ENABLE_DEV_TEMPERATURE_PUB)) {
        dev_temperature.msg = {
            .device_id = node_id,
            .temperature = static_cast<float>(CircuitPeriphery::temperature()),
            .error_flags = TemperatureErrorFlags_t(0),
        };
        dev_temperature.publish();
    }

    uint16_t error_flags = 0;
    if (CircuitPeriphery::overvoltage()) {
        error_flags |= (uint16_t)ERROR_FLAG_OVERVOLTAGE;
    }
    if (CircuitPeriphery::undervoltage()) {
        error_flags |= (uint16_t)ERROR_FLAG_UNDERVOLTAGE;
    }
    if (CircuitPeriphery::overcurrent()) {
        error_flags |= (uint16_t)ERROR_FLAG_OVERCURRENT;
    }

    if (bitmask & static_cast<uint8_t>(Bitmask::ENABLE_5V_PUB)) {
        circuit_status.msg = {
            .circuit_id = static_cast<uint16_t>(node_id * 10),
            .voltage = CircuitPeriphery::voltage_5v(),
            .current = CircuitPeriphery::current(),
            .error_flags = static_cast<CircuitStatusErrorFlags_t>(error_flags),
        };
        circuit_status.publish();
    }

    if (bitmask & static_cast<uint8_t>(Bitmask::ENABLE_VIN_PUB)) {
        circuit_status.msg = {
            .circuit_id = static_cast<uint16_t>(node_id * 10 + 1),
            .voltage = CircuitPeriphery::voltage_vin(),
            .current = CircuitPeriphery::current(),
            .error_flags = static_cast<CircuitStatusErrorFlags_t>(error_flags),
        };
        circuit_status.publish();
    }

    if (bitmask & static_cast<uint8_t>(Bitmask::ENABLE_HW_CHECKS)) {
        set_health(CircuitPeriphery::is_failure() ? Status::MINOR_FAILURE : Status::OK);
    } else {
        set_health(Status::OK);
    }
}
