/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_MODULES_IMU_HPP_
#define SRC_MODULES_IMU_HPP_

#include <numbers>
#include "module.hpp"
#include "publisher.hpp"
#include "drivers/mpu9250/mpu9250.hpp"
#include "common/FFT.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class ImuModule : public Module {
public:
    enum class Bitmask : uint8_t {
        DISABLED                    = 0,
        ENABLE_VIB_ESTIM            = 2,
        ENABLE_FFT_ACC              = 4,
        ENABLE_FFT_GYR              = 8,
        ENABLE_ALL_BY_DEFAULT       = 15,
    };
    ImuModule() : Module(256, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;
    void update_params() override;
    void get_vibration(std::array<float, 3> data);
    void update_accel_fft();
    void update_gyro_fft();

private:
    DronecanPublisher<AhrsRawImu> pub;
    DronecanPublisher<MagneticFieldStrength2> mag;
    Mpu9250 imu;
    FFT fft_accel;
    FFT fft_gyro;
    float vibration = 0.0f;
    bool initialized{false};
    bool enabled{false};
    uint8_t bitmask{0};
    uint16_t pub_timeout_ms{0};
    std::array<float, 3> gyro  = {0.0f, 0.0f, 0.0f};
    std::array<float, 3> accel = {0.0f, 0.0f, 0.0f};
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
