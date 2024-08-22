/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "imu.hpp"
#include "params.hpp"

REGISTER_MODULE(ImuModule)

void ImuModule::init() {
    initialized = imu.initialize();
    mode = Module::Mode::STANDY;
}

void ImuModule::update_params() {
    enabled = static_cast<bool>(paramsGetIntegerValue(PARAM_IMU_ENABLE));
    health = (!enabled || initialized) ? Module::Status::OK : Module::Status::MAJOR_FAILURE;
}

void ImuModule::spin_once() {
    if (!enabled || !initialized) {
        return;
    }

    std::array<int16_t, 3> mag_raw;
    if (imu.read_magnetometer(&mag_raw) >= 0) {
        mag.publish();
    }

    bool updated{false};

    std::array<int16_t, 3> accel_raw;
    if (imu.read_accelerometer(&accel_raw) >= 0) {
        pub.msg.accelerometer_latest[0] = raw_accel_to_meter_per_square_second(accel_raw[0]);
        pub.msg.accelerometer_latest[1] = raw_accel_to_meter_per_square_second(accel_raw[1]);
        pub.msg.accelerometer_latest[2] = raw_accel_to_meter_per_square_second(accel_raw[2]);
        updated = true;
    }

    std::array<int16_t, 3> gyro_raw;
    if (imu.read_gyroscope(&gyro_raw) >= 0) {
        pub.msg.rate_gyro_latest[0] = raw_gyro_to_rad_per_second(gyro_raw[0]);
        pub.msg.rate_gyro_latest[1] = raw_gyro_to_rad_per_second(gyro_raw[1]);
        pub.msg.rate_gyro_latest[2] = raw_gyro_to_rad_per_second(gyro_raw[2]);
        updated = true;
    }

    if (updated) {
        pub.msg.timestamp = HAL_GetTick() * 1000;
        pub.publish();
    }
}
