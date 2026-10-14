/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "board_status.hpp"
#include <limits>
#include "params.hpp"

REGISTER_MODULE(DronecanBoardStatus)

// Both circuits carry the flags of both rails, as DroneCAN nodes have always reported them.
static uint8_t to_dronecan_error_flags(uint8_t board_errors) {
    constexpr uint8_t OVERVOLTAGE = topics::BOARD_ERROR_V5_OVERVOLTAGE
                                    | topics::BOARD_ERROR_VIN_OVERVOLTAGE;
    constexpr uint8_t UNDERVOLTAGE = topics::BOARD_ERROR_V5_UNDERVOLTAGE
                                     | topics::BOARD_ERROR_VIN_UNDERVOLTAGE;

    uint8_t flags = ERROR_FLAG_CLEAR;
    if (board_errors & OVERVOLTAGE) {
        flags |= ERROR_FLAG_OVERVOLTAGE;
    }
    if (board_errors & UNDERVOLTAGE) {
        flags |= ERROR_FLAG_UNDERVOLTAGE;
    }
    if (board_errors & topics::BOARD_ERROR_OVERCURRENT) {
        flags |= ERROR_FLAG_OVERCURRENT;
    }
    return flags;
}

void DronecanBoardStatus::init() {
    set_mode(Mode::STANDBY);
}

void DronecanBoardStatus::update_params() {
    node_id = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID));
    bitmask = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_CRCT_BITMASK));
}

void DronecanBoardStatus::spin_once() {
    topics::BoardStatus status;
    if (!status_sub.update(&status)) {
        return;
    }

    if (is_bit_set(Bitmask::ENABLE_DEV_TEMPERATURE_PUB)) {
        dev_temperature.msg = {
            .device_id = node_id,
            .temperature = status.temperature,
            .error_flags = TemperatureErrorFlags_t(0),
        };
        dev_temperature.publish();
    }

    const auto flags = to_dronecan_error_flags(status.error_flags);
    if (is_bit_set(Bitmask::ENABLE_5V_PUB)) {
        const auto current = (status.current_rail == topics::CurrentRail::V5)
                             ? status.current : std::numeric_limits<float>::quiet_NaN();
        publish_circuit(static_cast<uint16_t>(node_id * 10), status.voltage_5v, current, flags);
    }
    if (is_bit_set(Bitmask::ENABLE_VIN_PUB)) {
        // Vin has always carried the current whatever rail the sensor is on
        publish_circuit(static_cast<uint16_t>(node_id * 10 + 1), status.voltage_vin, status.current,
                        flags);
    }

    auto failed = is_bit_set(Bitmask::ENABLE_HW_CHECKS) && status.error_flags != 0;
    set_health(failed ? Status::MINOR_FAILURE : Status::OK);
}

void DronecanBoardStatus::publish_circuit(uint16_t circuit_id,
                                          float voltage,
                                          float current,
                                          uint8_t error_flags) {
    circuit_status.msg = {
        .circuit_id = circuit_id,
        .voltage = voltage,
        .current = current,
        .error_flags = static_cast<CircuitStatusErrorFlags_t>(error_flags),
    };
    circuit_status.publish();
}
