/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_IMU_HPP_
#define SRC_MODULES_IMU_HPP_

// IMPORTANT: use some seed like "2345" here
#define RANDOM_SEED 2345

// Basically range can be any as it's just syntetic data and won't be near to something realistic
#define MAX_RND_RANGE 100
#define MIN_RND_RANGE 0

#include <numbers>
#include "module.hpp"
#include "publisher.hpp"
#include "drivers/mpu9250/mpu9250.hpp"
#include <random>

#ifdef __cplusplus
extern "C" {
#endif

// Much less memory and computing power consumption compared to the std::mt19937
std::minstd_rand rng(RANDOM_SEED);

std::uniform_int_distribution<int> dist(MIN_RND_RANGE, MAX_RND_RANGE);

class ImuModule : public Module {
public:
    enum class Bitmask : uint8_t {
        DISABLED                    = 0,
        ENABLE_VIB_ESTIM            = 2,
        ENABLE_FFT_ACC              = 4,
        ENABLE_FFT_GYR              = 8,
        ENABLE_ALL_BY_DEFAULT       = 15,
    };
    ImuModule() : Module(400.0, Protocol::DRONECAN) {}
    void init() override;
    bool isInitialised (void);

protected:
    void spin_once() override;
    void update_params() override;
    void get_vibration(std::array<float, 3> data);
    void fft();

private:
    DronecanPublisher<AhrsRawImu> pub;
    DronecanPublisher<MagneticFieldStrength2> mag;
    Mpu9250 imu;
    float vibration = 0.0f;
    bool initialized{false};
    bool enabled{false};
    uint8_t bitmask{0};
    uint16_t pub_timeout_ms{0};
    
    static constexpr float raw_gyro_to_rad_per_second(int16_t raw_gyro) {
        return raw_gyro * std::numbers::pi_v<float> / 131.0f / 180.0f;
    }

    static constexpr float raw_accel_to_meter_per_square_second(int16_t raw_accel) {
        return raw_accel * 9.80665f / 16384.0f;
    }
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_IMU_HPP_
