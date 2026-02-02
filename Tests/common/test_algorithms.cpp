/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include <gtest/gtest.h>
#include <cmath> // For std::fabs
#include <algorithm> // For std::clamp
#include <array>
#include "algorithms.hpp"


static constexpr float ABS_ERR = 0.1f;

class QuaternionTest : public ::testing::Test {
protected:
    float q[4];       // x, y, z, w
    float rpy[3];     // roll, pitch, yaw

    void SetUp() override {
        std::copy_n(std::initializer_list<float>{0.0f, 0.0f, 0.0f, 1.0f}.begin(), 4, q);
        std::copy_n(std::initializer_list<float>{0.0f, 0.0f, 0.0f}.begin(), 3, rpy);
    }
};

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



TEST_F(QuaternionTest, Normalize_Basic) {
    // Identity and simple scaling
    normalize_quaternion(q);
    EXPECT_NEAR(q[3], 1.0f, ABS_ERR);

    // Test over-sized and zero input
    std::copy_n(std::initializer_list<float>{0.0f, 2.0f, 0.0f, 0.0f}.begin(), 4, q); 
    normalize_quaternion(q);
    EXPECT_NEAR(q[1], 1.0f, ABS_ERR);
    
    std::copy_n(std::initializer_list<float>{0.0f, 0.0f, 0.0f, 0.0f}.begin(), 4, q);
    normalize_quaternion(q);
    EXPECT_FLOAT_EQ(q[0], 0.0f);
}

TEST_F(QuaternionTest, Normalize_Advanced) {
    // Verification tool: https://www.andre-gaschler.com/rotationconverter/ (normilizes automatically)
    std::copy_n(std::initializer_list<float>{100.0f, 546.0f, 234.0f, 1.0f}.begin(), 4, q); 
    normalize_quaternion(q);
    EXPECT_NEAR(q[0], 0.166f, ABS_ERR);
    EXPECT_NEAR(q[1], 0.906f, ABS_ERR);
    EXPECT_NEAR(q[2], 0.388f, ABS_ERR);
    EXPECT_NEAR(q[3], 0.0016601f, ABS_ERR);
}



TEST_F(QuaternionTest, Euler_PureAxes_90Deg) {
    // Identity
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[0], 0.0f, ABS_ERR);

    float val = std::sin(PI/4.0f);
    
    // Pure Roll (+90 deg)
    std::copy_n(std::initializer_list<float>{val, 0.0f, 0.0f, val}.begin(), 4, q);
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[0], PI_2, ABS_ERR);
    EXPECT_NEAR(rpy[1], 0.0f, ABS_ERR);
    
    // Pure Yaw (+90 deg)
    std::copy_n(std::initializer_list<float>{0.0f, 0.0f, val, val}.begin(), 4, q);
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[2], PI_2, ABS_ERR);
    EXPECT_NEAR(rpy[1], 0.0f, ABS_ERR);
}

TEST_F(QuaternionTest, Euler_ComplexRotation_ZYX_Correct) {
    // Verification tool: https://www.andre-gaschler.com/rotationconverter/ (ZYX euler transformation!)
    std::copy_n(std::initializer_list<float>{0.6334939f, 0.3418856f,-0.4323847f, 0.5429947f}.begin(), 4, q);
    normalize_quaternion(q);
    
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[0], 1.6633149f, LOOSE_ERR);
    EXPECT_NEAR(rpy[1], 1.1658162f, LOOSE_ERR);
    EXPECT_NEAR(rpy[2], -0.0925186f, LOOSE_ERR);


    std::copy_n(std::initializer_list<float>{0.34f, -0.95f,-0.3287f, 12}.begin(), 4, q);
    normalize_quaternion(q);
    
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[0], 0.0613147f, LOOSE_ERR);
    EXPECT_NEAR(rpy[1], -0.1561989f, LOOSE_ERR);
    EXPECT_NEAR(rpy[2], -0.0595695f, LOOSE_ERR);
}

TEST_F(QuaternionTest, Euler_GimbalLock_Singularities) {
    float val = std::sin(PI/4.0f);
    
    // North Pole (Pitch = +90 deg)
    std::copy_n(std::initializer_list<float>{0.0f, val, 0.0f, val}.begin(), 4, q); 
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[1], PI_2, ABS_ERR);
    EXPECT_FALSE(std::isnan(rpy[0]));
    EXPECT_FALSE(std::isnan(rpy[2]));
    
    // South Pole (Pitch = -90 deg)
    val = std::sin(-PI/4.0f);
    std::copy_n(std::initializer_list<float>{0.0f, val, 0.0f, std::cos(-PI/4.0f)}.begin(), 4, q);
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(rpy[1], -PI_2, ABS_ERR);
    EXPECT_FALSE(std::isnan(rpy[0]));
    EXPECT_FALSE(std::isnan(rpy[2]));
}

TEST_F(QuaternionTest, Euler_Denormalized_SafeGuard) {
    // Tests if the asin check (fabs(sinp) >= 1.0f) correctly handles float overruns
    float val = std::sin(PI/4.0f);
    std::copy_n(std::initializer_list<float>{0.0f, val + 0.0001f, 0.0f, val + 0.0001f}.begin(), 4, q); 
    quaternion_to_euler(q, rpy);
    
    // Should be clamped to PI/2, not NaN
    EXPECT_NEAR(rpy[1], PI_2, ABS_ERR);
    EXPECT_FALSE(std::isnan(rpy[1]));
}

TEST_F(QuaternionTest, Euler_180DegRotation) {
    // 180 degree rotation around X (Tests atan2 behavior with near-zero inputs)
    std::copy_n(std::initializer_list<float>{1.0f, 0.0f, 0.0f, 0.0f}.begin(), 4, q); 
    quaternion_to_euler(q, rpy);
    EXPECT_NEAR(std::abs(rpy[0]), PI, ABS_ERR);
    EXPECT_NEAR(rpy[1], 0.0f, ABS_ERR);
    EXPECT_NEAR(rpy[2], 0.0f, ABS_ERR);
}


TEST(CordicTest, SinCos_FullCircle) {
    float s, c;
    
    // 0 Degrees
    fast_sin_cos(0.0f, &s, &c);
    EXPECT_NEAR(s, 0.0f, ABS_ERR);
    EXPECT_NEAR(c, 1.0f, ABS_ERR);

    // 90 Degrees (PI/2)
    fast_sin_cos(PI_2, &s, &c);
    EXPECT_NEAR(s, 1.0f, ABS_ERR);
    EXPECT_NEAR(c, 0.0f, ABS_ERR);

    // 120 Degrees
    fast_sin_cos(120.0f * (PI / 180.0f), &s, &c);
    EXPECT_NEAR(s, 0.8660f, ABS_ERR);  // sin(120) = sqrt(3)/2
    EXPECT_NEAR(c, -0.5000f, ABS_ERR); // cos(120) = -0.5

    // 180 Degrees (PI)
    fast_sin_cos(PI, &s, &c);
    EXPECT_NEAR(s, 0.0f, ABS_ERR);
    EXPECT_NEAR(c, -1.0f, ABS_ERR);

    // -90 Degrees (-PI/2)
    fast_sin_cos(-PI_2, &s, &c);
    EXPECT_NEAR(s, -1.0f, ABS_ERR);
    EXPECT_NEAR(c, 0.0f, ABS_ERR);
}


TEST_F(QuaternionTest, EulerToQuat_Basic) {
    // Identity: 0, 0, 0 -> [0, 0, 0, 1]
    float angles_rpy[3] = {0.0f, 0.0f, 0.0f};
    euler_to_quaternion(angles_rpy, q);
    EXPECT_NEAR(q[0], 0.0f, ABS_ERR);
    EXPECT_NEAR(q[1], 0.0f, ABS_ERR);
    EXPECT_NEAR(q[2], 0.0f, ABS_ERR);
    EXPECT_NEAR(q[3], 1.0f, ABS_ERR);

    // Pure Yaw 90 deg: [0, 0, sin(45), cos(45)] -> [0, 0, 0.707, 0.707]
    angles_rpy[2] = 90.0f;
    // RPY: roll=0, pitch=0, yaw=90
    euler_to_quaternion(angles_rpy, q);
    EXPECT_NEAR(q[2], 0.7071f, ABS_ERR);
    EXPECT_NEAR(q[3], 0.7071f, ABS_ERR);

    // Pure Pitch 90 deg: [0, sin(45), 0, cos(45)] -> [0, 0.707, 0, 0.707]
    angles_rpy[2] = 0.0f;
    angles_rpy[1] = 90.0f;
    // RPY: roll=0, pitch=90, yaw=0
    euler_to_quaternion(angles_rpy, q);
    EXPECT_NEAR(q[1], 0.7071f, ABS_ERR);
    EXPECT_NEAR(q[3], 0.7071f, ABS_ERR);
}

/**
 * Round-trip consistency: Euler -> Quat -> Euler
 */
TEST_F(QuaternionTest, Euler_RoundTrip) {
    float input_rpy[3] = {30.0f, -20.0f, 45.0f}; // Degrees
    float output_rpy[3]; // Radians

    euler_to_quaternion(input_rpy, q);

    quaternion_to_euler(q, output_rpy);

    rad_to_deg_array(output_rpy);

    EXPECT_NEAR(output_rpy[0], input_rpy[0], 0.5f); // Roll
    EXPECT_NEAR(output_rpy[1], input_rpy[1], 0.5f); // Pitch
    EXPECT_NEAR(output_rpy[2], input_rpy[2], 0.5f); // Yaw
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
