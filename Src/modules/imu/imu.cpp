/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "imu.hpp"
#include "params.hpp"

REGISTER_MODULE(ImuModule)

void ImuModule::init() {
    logger = Logging("IMU");
    bool imu_initialized = imu.initialize();
    mode = Module::Mode::STANDBY;
    uint16_t num_axes = 3;
    uint16_t window_size = 512;
    bool fft_initialized = fft.init(window_size, num_axes, 1000.0f/period_ms);
    fft.fft_min_freq = 10;
    initialized = imu_initialized && fft_initialized;
}

void ImuModule::update_params() {
    enabled = static_cast<bool>(paramsGetIntegerValue(PARAM_IMU_ENABLE));
    health = (!enabled || initialized) ? Module::Status::OK : Module::Status::MAJOR_FAILURE;
    if (enabled) {
        mode = initialized ? Mode::STANDBY : Mode::INITIALIZATION;
    }
}

void ImuModule::spin_once() {
    if (!enabled || !initialized) {
        return;
    }

    std::array<int16_t, 3> mag_raw;
    if (imu.read_magnetometer(&mag_raw) >= 0) {
        mag.publish();
    }

    bool updated[2]{false, false};

    std::array<int16_t, 3> accel_raw = {0, 0, 0};
    std::array<int16_t, 3> gyro_raw  = {0, 0, 0};

    if (imu.read_gyroscope(&gyro_raw) >= 0) {
        float gyro[3] = {
                raw_gyro_to_rad_per_second(gyro_raw[0]),
                raw_gyro_to_rad_per_second(gyro_raw[1]),
                raw_gyro_to_rad_per_second(gyro_raw[2])};
        pub.msg.rate_gyro_latest[0] = gyro[0];
        pub.msg.rate_gyro_latest[1] = gyro[1];
        pub.msg.rate_gyro_latest[2] = gyro[2];
        fft.update(gyro);
        updated[0] = true;
    }

    if (imu.read_accelerometer(&accel_raw) >= 0) {
        float accel[3] = {
                raw_accel_to_meter_per_square_second(accel_raw[0]),
                raw_accel_to_meter_per_square_second(accel_raw[1]),
                raw_accel_to_meter_per_square_second(accel_raw[2])};
        pub.msg.accelerometer_latest[0] = accel[0];
        pub.msg.accelerometer_latest[1] = accel[1];
        pub.msg.accelerometer_latest[2] = accel[2];
        updated[1] = true;
    }

    if (updated[0] && updated[1]) {
        pub.msg.timestamp = HAL_GetTick() * 1000;
        pub.publish();
    }

    static uint32_t prev_time = HAL_GetTick();
    if (prev_time + 1000 < HAL_GetTick() || fft._fft_updated[0]) {
        prev_time = HAL_GetTick();
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "peaks\nfreq: %d %d %d\n snrs: %d %d %d",
                (int)(fft.peak_frequencies[0][0]),
                (int)(fft.peak_frequencies[1][0]),
                (int)(fft.peak_frequencies[2][0]),
                (int)(fft.peak_snr[0][0]),
                (int)(fft.peak_snr[1][0]),
                (int)(fft.peak_snr[2][0])
                );
        logger.log_info(buffer);
    }
}
