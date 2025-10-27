/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_MODULES_IMU_HPP_
#define SRC_MODULES_IMU_HPP_

#define FFT_MIN_FREQ 0.1f
#define WINDOW_SIZE 256
#define NUM_AXES 3
#define SAMPLE_RATE_HZ 256
// IMPORTANT NOTE: 
// If you want to see oscilation data itself in a way that program sees it then
// set imu.pub_frequency to the GENERATOR_SAMPLE_HZ frequency, so publisher would be able to publish data on time
#define GENERATOR_SAMPLE_HZ 256


#include <numbers>
#include <random>
#include "module.hpp"
#include "libdcnode/publisher.hpp"
#include "drivers/mpu9250/mpu9250.hpp"
#include "common/FFT.hpp"
#include "common/logging.hpp"
#include "common/oscillation_generator.hpp"

#ifdef __cplusplus
extern "C" {
#endif
// TODO (ilyha_dev): make lazy FFTs and generator initialisation as it's big classes
class ImuModule : public Module {
public:
    enum class Publisher_bitmask : uint8_t {
        DISABLED                    = 0,
        ENABLE_VIB_ESTIM            = 2,
        ENABLE_FFT_ACC              = 4,
        ENABLE_FFT_GYR              = 8,
        ENABLE_ALL_BY_DEFAULT       = 15,
    };

    enum class Data_bitmast : uint8_t {
        DISABLED                    = 0,
        ENABLE_SYNTH_GEN            = 1,
        ENABLE_FIFO_READINGS        = 2,
        ENABLE_REG_READINGS         = 4,
    };

    ImuModule() : Module(256, Protocol::DRONECAN) {}
    void init() override;
    void set_initialize (bool initialize);

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

    SinSignalGenerator accel_signals_generator {GENERATOR_SAMPLE_HZ};
    uint16_t gen_freq{0};
    uint16_t gen_amplitude{0};

    float vibration = 0.0f;
    bool initialized{false};
    bool enabled{false};
    uint8_t publisher_bitmask{0};
    uint8_t data_bitmask{0};
    uint16_t pub_timeout_ms{0};

    std::array<float, NUM_AXES> gyro  = {0.0f, 0.0f, 0.0f};
    std::array<float, NUM_AXES> accel = {0.0f, 0.0f, 0.0f};
    float temperature{0};

    void process_random_gen (std::array<bool, 2>& updated);
    void process_real_fifo (std::array<bool, 2>& updated);
    void process_real_register (std::array<bool, 2>& updated);
    static constexpr float raw_gyro_to_rad_per_second(int16_t raw_gyro) {
        return raw_gyro * std::numbers::pi_v<float> / 131.0f / 180.0f;
    }

    static constexpr float raw_accel_to_meter_per_square_second(int16_t raw_accel) {
        return raw_accel * 9.80665f / 16384.0f;
    }
    constexpr int16_t raw_temp_convert_to_celsius (int16_t raw_temp){
        // IMPORTANT: this temperature is "die" (iternal) temperature of the MPU9250. It DOES NOT refer to ambient temperature
        // 21 derived from Room Temp Offset 
        temperature = (float)raw_temp / 333.87f + 21.0;
        // Simple and low cost rounding
        return (static_cast<int16_t>(temperature + (temperature >= 0 ? 0.5f : -0.5f)));
    }

};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_IMU_HPP_
