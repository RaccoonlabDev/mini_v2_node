/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "circuit_status.hpp"
#include "periphery/adc/circuit_periphery.hpp"

REGISTER_MODULE(DronecanCircuitStatus)

void DronecanCircuitStatus::init() {
    mode = Module::Mode::STANDY;
}

void DronecanCircuitStatus::spin_once() {
    dev_temperature.msg.temperature = CircuitPeriphery::temperature();
    dev_temperature.publish();

    circuit_status.msg.voltage = CircuitPeriphery::voltage_vin();
    circuit_status.msg.current = CircuitPeriphery::current();
    uint16_t error_flags = 0;
    error_flags |= CircuitPeriphery::overvoltage() * (uint16_t)ERROR_FLAG_OVERVOLTAGE;
    error_flags |= CircuitPeriphery::undervoltage() * (uint16_t)ERROR_FLAG_UNDERVOLTAGE;
    error_flags |= CircuitPeriphery::overcurrent() * (uint16_t)ERROR_FLAG_OVERCURRENT;
    circuit_status.msg.error_flags = static_cast<CircuitStatusErrorFlags_t>(error_flags);
    circuit_status.publish();

    health = CircuitPeriphery::is_failure() ? Status::MINOR_FAILURE : Status::OK;
}
