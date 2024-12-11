/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <math.h>
#include "imu.hpp"
#include "params.hpp"
#include "common/logging.hpp"

REGISTER_MODULE(ImuModule)

void ImuModule::init() {
    initialized = imu.initialize();
    mode = Module::Mode::STANDBY;
    fft_accel.init(256, 3, 256);
    fft_accel.fft_min_freq = 20;
    fft_gyro.init(256, 3, 256);
    fft_gyro.fft_min_freq = 20;
}

void ImuModule::update_params() {
    auto pub_frequency = static_cast<uint16_t>(
                                paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_PUB_FREQUENCY));
    pub_timeout_ms = pub_frequency == 0 ? 0 : 1000 / pub_frequency;
    bitmask = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_MODE_BITMASK));
    health = (!bitmask || initialized) ? Module::Status::OK : Module::Status::MAJOR_FAILURE;
    if (bitmask) {
        mode = initialized ? Mode::STANDBY : Mode::INITIALIZATION;
    }
}

void ImuModule::spin_once() {
    if (!initialized) {
        initialized = imu.initialize();
    }
    if (!bitmask || !initialized) {
        return;
    }

    std::array<int16_t, 3> mag_raw;
    if (imu.read_magnetometer(&mag_raw) >= 0) {
        mag.publish();
    }

    bool updated[2]{false, false};

    std::array<int16_t, 3>  accel_raw = {0, 0, 0};
    std::array<int16_t, 3>  gyro_raw  = {0, 0, 0};
    if (imu.read_gyroscope(&gyro_raw) >= 0) {
        gyro = {
                raw_gyro_to_rad_per_second(gyro_raw[0]),
                raw_gyro_to_rad_per_second(gyro_raw[1]),
                raw_gyro_to_rad_per_second(gyro_raw[2])};
        pub.msg.rate_gyro_latest[0] = gyro[0];
        pub.msg.rate_gyro_latest[1] = gyro[1];
        pub.msg.rate_gyro_latest[2] = gyro[2];
        updated[0] = true;
        update_gyro_fft();
    }

    if (imu.read_accelerometer(&accel_raw) >= 0) {
        accel = {
                raw_accel_to_meter_per_square_second(accel_raw[0]),
                raw_accel_to_meter_per_square_second(accel_raw[1]),
                raw_accel_to_meter_per_square_second(accel_raw[2])};
        get_vibration(accel);
        pub.msg.accelerometer_latest[0] = accel[0];
        pub.msg.accelerometer_latest[1] = accel[1];
        pub.msg.accelerometer_latest[2] = accel[2];
        updated[1] = true;
        update_accel_fft();
    }

    if (pub_timeout_ms != 0 && HAL_GetTick() - pub.msg.timestamp / 1000 > pub_timeout_ms) {
        if (updated[0] && updated[1]) {
            pub.msg.timestamp = HAL_GetTick() * 1000;
            pub.publish();
        }
    }
}

void ImuModule::get_vibration(std::array<float, 3> data) {
    if (!(bitmask & static_cast<std::underlying_type_t<Bitmask>>(Bitmask::ENABLE_VIB_ESTIM))) {
        return;
    }
    float diff_magnitude = 0.0f;
    for (uint8_t i = 0; i < 3; i++) {
        diff_magnitude += std::pow(data[i] - pub.msg.accelerometer_latest[i], 2);
    }
    vibration = 0.99f * vibration + 0.01f * std::sqrt(diff_magnitude);
    pub.msg.integration_interval = vibration;
    return;
}

void ImuModule::update_accel_fft() {
    if (!(bitmask & static_cast<std::underlying_type_t<Bitmask>>(Bitmask::ENABLE_FFT_ACC))) {
        return;
    }
    fft_accel.update(accel.data());
    pub.msg.accelerometer_integral[0] = fft_accel.dominant_frequency;
    pub.msg.accelerometer_integral[1] = fft_accel.dominant_mag * 1000;
    pub.msg.accelerometer_integral[2] = fft_accel.dominant_snr;
}

void ImuModule::update_gyro_fft() {
    if (!(bitmask & static_cast<std::underlying_type_t<Bitmask>>(Bitmask::ENABLE_FFT_GYR))) {
        return;
    }
    // fft_gyro.update(gyro.data());
    pub.msg.rate_gyro_integral[0] = fft_gyro.dominant_frequency;
    pub.msg.rate_gyro_integral[1] = fft_gyro.dominant_mag * 1000;
    pub.msg.rate_gyro_integral[2] = fft_gyro.dominant_snr;
}
