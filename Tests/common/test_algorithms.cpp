/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <gtest/gtest.h>
#include <cmath> // For std::fabs
#include <algorithm> // For std::clamp

static constexpr auto ABS_ERR = 0.1f;

float mapFloat(float value, float in_min, float in_max, float out_min, float out_max) {
    if (std::fabs(in_min - in_max) < 1e-6f) {
        return out_min;
    }

    float output = out_min + (value - in_min) / (in_max - in_min) * (out_max - out_min);

    return std::clamp(output, std::min(out_min, out_max), std::max(out_min, out_max));
}

TEST(MapFloatTest, raw_command_esc) {
    static constexpr auto IN_MIN = 0;
    static constexpr auto IN_MAX = 8191;
    static constexpr auto OUT_MIN = 1000;
    static constexpr auto OUT_MAX = 2000;

    EXPECT_NEAR(mapFloat(-1,        IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(0,         IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(4095,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), 1500.0f, ABS_ERR);
    EXPECT_NEAR(mapFloat(8191,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
    EXPECT_NEAR(mapFloat(8192,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
}

TEST(MapFloatTest, raw_command_servo_inverted_with_ofsset) {
    static constexpr auto IN_MIN = 0;
    static constexpr auto IN_MAX = 8191;
    static constexpr auto OUT_MIN = 1600;
    static constexpr auto OUT_MAX = 1800;

    EXPECT_NEAR(mapFloat(-1,        IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(0,         IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(4095,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), 1700.0f, ABS_ERR);
    EXPECT_NEAR(mapFloat(8191,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
    EXPECT_NEAR(mapFloat(8192,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
}

TEST(MapFloatTest, actuator_command_servo) {
    static constexpr auto IN_MIN = -1.0f;
    static constexpr auto IN_MAX = +1.0f;
    static constexpr auto OUT_MIN = 1200;
    static constexpr auto OUT_MAX = 1400;

    EXPECT_NEAR(mapFloat(-1.01f,    IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(-1.0f,     IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(0.0f,      IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), 1300.0f, ABS_ERR);
    EXPECT_NEAR(mapFloat(+1.0f,     IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
    EXPECT_NEAR(mapFloat(+1.01f,    IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
}

TEST(MapFloatTest, inveted_air_servo_mapping) {
    static constexpr auto IN_MIN = 2000.0f;
    static constexpr auto IN_MAX = 1000.0f;
    static constexpr auto OUT_MIN = 1700;
    static constexpr auto OUT_MAX = 1900;

    EXPECT_NEAR(mapFloat(2001.0f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(2000.0f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MIN, ABS_ERR);
    EXPECT_NEAR(mapFloat(1750.0f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), 1750.0f, ABS_ERR);
    EXPECT_NEAR(mapFloat(1250.0f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), 1850.0f, ABS_ERR);
    EXPECT_NEAR(mapFloat(1000.0f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
    EXPECT_NEAR(mapFloat(999.00f,   IN_MIN, IN_MAX, OUT_MIN, OUT_MAX), OUT_MAX, ABS_ERR);
}

TEST(MapFloatTest, wrong_input) {
    EXPECT_NEAR(mapFloat(1500, 1000, 1000, 1000, 2000), 1000, ABS_ERR);
}

TEST(MapFloatTest, wrong_output) {
    EXPECT_NEAR(mapFloat(0.5f, 0.0f, 1.0f, 1000, 1000), 1000, ABS_ERR);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
