/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_RAW_CAN_BOARD_STATUS_FRAME_HPP_
#define SRC_MODULES_RAW_CAN_BOARD_STATUS_FRAME_HPP_

#include <array>
#include <cmath>
#include <cstdint>
#include "topics/board_status.hpp"

/**
 * @brief The frame of the raw CAN example: 11-bit id BOARD_STATUS_BASE_ID + node id,
 * 8 bytes, little-endian.
 *
 * | Bytes | Field       | Encoding                                   |
 * |-------|-------------|--------------------------------------------|
 * | 0-1   | voltage_5v  | uint16, mV, 0xFFFF if not available        |
 * | 2-3   | voltage_vin | uint16, mV, 0xFFFF if not available        |
 * | 4-5   | current     | int16, 10 mA, 0x8000 if not available      |
 * | 6     | temperature | int8, °C, 0x80 if not available            |
 * | 7     | error flags | topics::BoardError bits                    |
 */
namespace raw_can {

constexpr uint32_t BOARD_STATUS_BASE_ID{0x100};
constexpr uint8_t FRAME_SIZE{8};
constexpr uint16_t MILLIVOLTS_NOT_AVAILABLE{UINT16_MAX};
constexpr int16_t CENTIAMPS_NOT_AVAILABLE{INT16_MIN};
constexpr int8_t CELSIUS_NOT_AVAILABLE{INT8_MIN};

// Round to the nearest unit and saturate; the extreme value of each field means "not available".

inline uint16_t encode_millivolts(float volts) {
    if (std::isnan(volts)) {
        return MILLIVOLTS_NOT_AVAILABLE;
    }
    const float millivolts = std::round(volts * 1000.0F);
    return static_cast<uint16_t>(std::fmin(std::fmax(millivolts, 0.0F), UINT16_MAX - 1.0F));
}

inline int16_t encode_centiamps(float amps) {
    if (std::isnan(amps)) {
        return CENTIAMPS_NOT_AVAILABLE;
    }
    const float centiamps = std::round(amps * 100.0F);
    return static_cast<int16_t>(std::fmin(std::fmax(centiamps, -INT16_MAX), INT16_MAX));
}

inline int8_t encode_celsius(float kelvin) {
    if (std::isnan(kelvin)) {
        return CELSIUS_NOT_AVAILABLE;
    }
    const float celsius = std::round(kelvin - 273.15F);
    return static_cast<int8_t>(std::fmin(std::fmax(celsius, -INT8_MAX), INT8_MAX));
}

inline std::array<uint8_t, FRAME_SIZE> encode_board_status(const topics::BoardStatus& status) {
    const uint16_t v5 = encode_millivolts(status.voltage_5v);
    const uint16_t vin = encode_millivolts(status.voltage_vin);
    const auto current = static_cast<uint16_t>(encode_centiamps(status.current));
    return {
        static_cast<uint8_t>(v5 & 0xFFU),
        static_cast<uint8_t>(v5 >> 8U),
        static_cast<uint8_t>(vin & 0xFFU),
        static_cast<uint8_t>(vin >> 8U),
        static_cast<uint8_t>(current & 0xFFU),
        static_cast<uint8_t>(current >> 8U),
        static_cast<uint8_t>(encode_celsius(status.temperature)),
        status.error_flags,
    };
}

}  // namespace raw_can

#endif  // SRC_MODULES_RAW_CAN_BOARD_STATUS_FRAME_HPP_
