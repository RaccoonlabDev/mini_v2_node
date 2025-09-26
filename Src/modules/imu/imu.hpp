/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_MODULES_IMU_HPP_
#define SRC_MODULES_IMU_HPP_

#define FFT_MIN_FREQ 5
#define WINDOW_SIZE 64
#define NUM_AXES 3
#define SAMPLE_RATE_HZ 64


#define GENERATOR_SAMPLE_HZ 64
#define GENERATOR_FREQ_HZ   20
#define GENERATOR_AMPLITUDE 10

#include <numbers>
#include <random>
#include "module.hpp"
#include "publisher.hpp"
#include "drivers/mpu9250/mpu9250.hpp"
#include "common/FFT.hpp"
#include "common/logging.hpp"
#include "common/oscillation_generator.hpp"

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
    Logging logger{"IMU"};
    Mpu9250 imu;
    FFT fft_accel;
    FFT fft_gyro;
    // TODO: probably too expensive to make 6 distinct generators
    // May be mathematical solution to derive from one generator two different frequencies
    /*{SinSignalGenerator{GENERATOR_SAMPLE_HZ, GENERATOR_FREQ_HZ, GENERATOR_AMPLITUDE},
                                                                    SinSignalGenerator{GENERATOR_SAMPLE_HZ, GENERATOR_FREQ_HZ, GENERATOR_AMPLITUDE},
                                                                    SinSignalGenerator{GENERATOR_SAMPLE_HZ, GENERATOR_FREQ_HZ, GENERATOR_AMPLITUDE}} */
    SinSignalGenerator accel_signals_generator {GENERATOR_SAMPLE_HZ, GENERATOR_FREQ_HZ, GENERATOR_AMPLITUDE};

    SinSignalGenerator  gyro_signals_generator {GENERATOR_SAMPLE_HZ, GENERATOR_FREQ_HZ, GENERATOR_AMPLITUDE};
    float vibration = 0.0f;
    bool initialized{false};
    bool enabled{false};
    uint8_t bitmask{0};
    uint16_t pub_timeout_ms{0};
    std::array<float, NUM_AXES> gyro  = {0.0f, 0.0f, 0.0f};
    std::array<float, NUM_AXES> accel = {0.0f, 0.0f, 0.0f};
    float temp{0};
    static constexpr float raw_gyro_to_rad_per_second(int16_t raw_gyro) {
        return raw_gyro * std::numbers::pi_v<float> / 131.0f / 180.0f;
    }

    static constexpr float raw_accel_to_meter_per_square_second(int16_t raw_accel) {
        return raw_accel * 9.80665f / 16384.0f;
    }
    constexpr int16_t raw_temp_convert_to_celsius (int16_t raw_temp){
        // IMPORTANT: this temperature is "die" (iternal) temperature of the MPU9250. It DOES NOT refer to ambient temperature
        // 21 derived from Room Temp Offset 
        temp = (float)raw_temp / 333.87f + 21.0;
        // Simple and low cost rounding
        return (static_cast<int16_t>(temp + (temp >= 0 ? 0.5f : -0.5f)));
    }
    void perform_logging_dronecan ();

};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_IMU_HPP_
