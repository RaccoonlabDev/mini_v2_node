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
#include "common/logging.hpp"

class DronecanCircuitStatus : public Module {
public:
    enum class Bitmask : uint8_t {
        DISABLED                    = 0,
        ENABLE_DEV_TEMPERATURE_PUB  = 1,
        ENABLE_5V_PUB               = 2,
        ENABLE_VIN_PUB              = 4,
        ENABLE_HW_CHECKS            = 8,
        ENABLE_ALL_BY_DEFAULT       = 15,
    };

    DronecanCircuitStatus() : Module(2.0f, Protocol::DRONECAN) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    DronecanPublisher<CircuitStatus_t> circuit_status;
    DronecanPublisher<Temperature_t> dev_temperature;

    uint8_t node_id{0};
    uint8_t bitmask{0};

    static inline Logging logger{"CRCT"};
};

#endif  // SRC_MODULES_CIRCUIT_STATUS_DRONECAN_CIRCUIT_STATUS_HPP_
