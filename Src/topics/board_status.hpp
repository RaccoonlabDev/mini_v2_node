/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_TOPICS_BOARD_STATUS_HPP_
#define SRC_TOPICS_BOARD_STATUS_HPP_

#include <cstdint>
#include "common/datapool.hpp"

/**
 * @brief The board standard: what every node board measures about itself.
 *
 * A quantity the board can't measure is NaN, and its error flags are never set: a micro board has
 * no DC-DC and so no Vin, many boards have no current sensor. The limits behind the flags are in
 * drivers/board_monitor.
 */
namespace topics {

enum BoardError : uint8_t {
    BOARD_ERROR_V5_UNDERVOLTAGE     = 1 << 0,
    BOARD_ERROR_V5_OVERVOLTAGE      = 1 << 1,
    BOARD_ERROR_VIN_UNDERVOLTAGE    = 1 << 2,
    BOARD_ERROR_VIN_OVERVOLTAGE     = 1 << 3,
    BOARD_ERROR_OVERCURRENT         = 1 << 4,
    BOARD_ERROR_OVERHEAT            = 1 << 5,
};

// The rail the board's only current sensor is on
enum class CurrentRail : uint8_t {
    NONE,
    V5,
    VIN,
};

struct BoardStatus {
    float temperature;              // K, MCU internal sensor
    float voltage_5v;               // V, the 5 V rail after the DC-DC
    float voltage_vin;              // V, the supply before the DC-DC
    float current;                  // A, on current_rail
    CurrentRail current_rail;
    uint16_t hardware_version_adc;  // raw ADC code of the version divider, 0 if there is none
    uint8_t error_flags;            // BoardError bits
};

struct BoardStatusTopic : datapool::Topic<BoardStatus> {};

}  // namespace topics

#endif  // SRC_TOPICS_BOARD_STATUS_HPP_
