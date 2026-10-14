/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_BACKENDS_DRONECAN_BOARD_STATUS_BOARD_STATUS_HPP_
#define SRC_MODULES_BACKENDS_DRONECAN_BOARD_STATUS_BOARD_STATUS_HPP_

#include "common/datapool.hpp"
#include "common/module.hpp"
#include "libdcnode/publisher.hpp"
#include "topics/board_status.hpp"

/**
 * @brief Puts topics::BoardStatusTopic on DroneCAN and reports the board health.
 * DroneCAN has no message for a board, so it goes out as dev.Temperature and two CircuitStatus:
 * circuit node_id * 10 for 5 V and node_id * 10 + 1 for Vin.
 */
class DronecanBoardStatus : public Module {
public:
    enum class Bitmask : uint8_t {
        DISABLED                    = 0,
        ENABLE_DEV_TEMPERATURE_PUB  = 1,
        ENABLE_5V_PUB               = 2,
        ENABLE_VIN_PUB              = 4,
        ENABLE_HW_CHECKS            = 8,
        ENABLE_ALL_BY_DEFAULT       = 15,
    };

    // The topic comes at 2 Hz: poll faster, so every sample goes out once and soon.
    DronecanBoardStatus() : Module(20.0F, Protocol::DRONECAN) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    bool is_bit_set(Bitmask bit) const {
        return bitmask & static_cast<uint8_t>(bit);
    }
    void publish_circuit(uint16_t circuit_id, float voltage, float current, uint8_t error_flags);

    datapool::Subscriber<topics::BoardStatusTopic> status_sub;

    DronecanPublisher<CircuitStatus_t> circuit_status;
    DronecanPublisher<Temperature_t> dev_temperature;

    uint8_t node_id{0};
    uint8_t bitmask{0};
};

#endif  // SRC_MODULES_BACKENDS_DRONECAN_BOARD_STATUS_BOARD_STATUS_HPP_
