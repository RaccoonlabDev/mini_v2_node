/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_CIRCUIT_STATUS_DRONECAN_CIRCUIT_STATUS_HPP_
#define SRC_MODULES_CIRCUIT_STATUS_DRONECAN_CIRCUIT_STATUS_HPP_

#include "common/module.hpp"
#include "publisher.hpp"

class DronecanCircuitStatus : public Module {
public:
    DronecanCircuitStatus() : Module(2.0f, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;

private:
    DronecanPublisher<CircuitStatus_t> circuit_status;
    DronecanPublisher<Temperature_t> dev_temperature;
};

#endif  // SRC_MODULES_CIRCUIT_STATUS_DRONECAN_CIRCUIT_STATUS_HPP_
