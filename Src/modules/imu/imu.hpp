/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_IMU_HPP_
#define SRC_MODULES_IMU_HPP_

#include <numbers>
#include "module.hpp"
#include "publisher.hpp"
#include "drivers/mpu9250/mpu9250.hpp"
#include "common/FFT.hpp"
#include "common/logging.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class ImuModule : public Module {
public:
    ImuModule() : Module(00.0, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;
    void update_params() override;

private:
    DronecanPublisher<AhrsRawImu> pub;
    DronecanPublisher<MagneticFieldStrength2> mag;
    Mpu9250 imu;
    FFT fft;
    Logging logger{"IMU"};
    bool initialized{false};
    bool enabled{false};

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
