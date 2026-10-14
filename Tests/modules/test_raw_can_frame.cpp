/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <cstdint>
#include "modules/backends/raw_can/board_status/frame.hpp"

namespace {

using Frame = std::array<uint8_t, raw_can::FRAME_SIZE>;

TEST(RawCanFrame, EncodesTheDocumentedLayout) {
    const topics::BoardStatus status{
        .temperature = 308.15F,
        .voltage_5v = 5.0F,
        .voltage_vin = 12.5F,
        .current = -0.25F,
        .current_rail = topics::CurrentRail::VIN,
        .hardware_version_adc = 2420,
        .error_flags = topics::BOARD_ERROR_VIN_OVERVOLTAGE,
    };

    // 5000 mV, 12500 mV, -25 x 10 mA, 35 °C, the flags
    const Frame expected{0x88, 0x13, 0xD4, 0x30, 0xE7, 0xFF, 35,
                         topics::BOARD_ERROR_VIN_OVERVOLTAGE};
    EXPECT_EQ(raw_can::encode_board_status(status), expected);
}

TEST(RawCanFrame, RoundsToTheNearestUnit) {
    EXPECT_EQ(raw_can::encode_millivolts(4.9996F), 5000);
    EXPECT_EQ(raw_can::encode_millivolts(4.9994F), 4999);
    EXPECT_EQ(raw_can::encode_centiamps(-0.004F), 0);
    EXPECT_EQ(raw_can::encode_centiamps(1.236F), 124);
    EXPECT_EQ(raw_can::encode_celsius(273.15F + 24.6F), 25);
}

TEST(RawCanFrame, NanIsNotAvailable) {
    EXPECT_EQ(raw_can::encode_millivolts(NAN), raw_can::MILLIVOLTS_NOT_AVAILABLE);
    EXPECT_EQ(raw_can::encode_centiamps(NAN), raw_can::CENTIAMPS_NOT_AVAILABLE);
    EXPECT_EQ(raw_can::encode_celsius(NAN), raw_can::CELSIUS_NOT_AVAILABLE);
}

TEST(RawCanFrame, SaturatesWithoutHittingTheNotAvailableValues) {
    EXPECT_EQ(raw_can::encode_millivolts(-1.0F), 0);
    EXPECT_EQ(raw_can::encode_millivolts(70.0F), raw_can::MILLIVOLTS_NOT_AVAILABLE - 1);
    EXPECT_EQ(raw_can::encode_millivolts(INFINITY), raw_can::MILLIVOLTS_NOT_AVAILABLE - 1);
    EXPECT_EQ(raw_can::encode_centiamps(400.0F), INT16_MAX);
    EXPECT_EQ(raw_can::encode_centiamps(-INFINITY), -INT16_MAX);
    EXPECT_EQ(raw_can::encode_celsius(0.0F), -INT8_MAX);
    EXPECT_EQ(raw_can::encode_celsius(500.0F), INT8_MAX);
}

}  // namespace
