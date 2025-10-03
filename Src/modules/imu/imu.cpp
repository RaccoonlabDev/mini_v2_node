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
    set_mode(Mode::STANDBY);
    fft_accel.init(WINDOW_SIZE, NUM_AXES, SAMPLE_RATE_HZ);
    fft_accel.fft_min_freq = FFT_MIN_FREQ;
    fft_gyro.init(WINDOW_SIZE, NUM_AXES, SAMPLE_RATE_HZ);
    fft_gyro.fft_min_freq = FFT_MIN_FREQ;
}

void ImuModule::update_params() {
    auto pub_frequency = static_cast<uint16_t>(
                                paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_PUB_FREQUENCY));
    pub_timeout_ms = pub_frequency == 0 ? 0 : 1000 / pub_frequency;
    bitmask = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_MODE_BITMASK));
    set_health((!bitmask || initialized) ? Module::Status::OK : Module::Status::MAJOR_FAILURE);
    gen_amplitude = static_cast<uint16_t>(paramsGetIntegerValue(IntParamsIndexes::SYNTHETIC_AMPLITUDE));
    gen_freq = static_cast<uint16_t>(paramsGetIntegerValue(IntParamsIndexes::SYNTHETIC_FREQ_GEN));
    if (bitmask) {
        set_mode(initialized ? Mode::STANDBY : Mode::INITIALIZATION);
    }
}

void ImuModule::spin_once() {
    bool generateRandom = ((0 == static_cast<uint16_t>
        (paramsGetIntegerValue(IntParamsIndexes::SYNTHETIC_GEN)))  ? true : false);

    bool updated[2]{false, false};
    // If synthetic accel is turned on (value is 0) then we need to go further to data generation
    if (!generateRandom) {
        if (!bitmask || !initialized) {
            return;
        }
        // Usual reading routine
        std::array<int16_t, NUM_AXES> mag_raw;
        if (imu.read_magnetometer(&mag_raw) >= 0) {
            mag.publish();
        }
        std::array<int16_t, NUM_AXES>  accel_raw = {0, 0, 0};
        std::array<int16_t, NUM_AXES>  gyro_raw  = {0, 0, 0};
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
    } else {
        // TODO(ilyha_dev): ideally implement it's own message type for synthetic data
        // Motivation: relevant idea iff we want to generate only one oscilating axis
        // i.e. put on dronecan only one axis which is used for oscilations - others are redundant
        // Otherwise (if motivation is not relevant): make lots of params for dronecan for each axis
        // i.e.
        // GENERATOR_SAMPLE_HZ for 3 gyro and 3 accel axis
        // GENERATOR_FREQ_HZ   for 3 gyro and 3 accel axis
        // GENERATOR_AMPLITUDE for 3 gyro and 3 accel axis

        // Set values for to generate
        accel_signals_generator.setAmpl(gen_amplitude);
        accel_signals_generator.setFreq(gen_freq);
        // Here we generate random value for oscilations on acceleration OX axis.
        static uint64_t last_sample_time_ms = 0;
        // FFT excpects data to be given in certain rate
        // as spin once called with unknown (really high) frequency i made this interval
        uint64_t sample_interval_ms = 1000 / GENERATOR_SAMPLE_HZ;
        uint64_t current_time = HAL_GetTick();
        if (current_time - last_sample_time_ms >= sample_interval_ms) {
            last_sample_time_ms = HAL_GetTick();
            /*pub.msg.rate_gyro_latest[0] = 0;
            pub.msg.rate_gyro_latest[1] = 0;
            pub.msg.rate_gyro_latest[2] = 0;*/
            // Set whole array to 0 using memset
            memset(pub.msg.rate_gyro_latest, 0, sizeof(pub.msg.rate_gyro_latest)*sizeof(pub.msg.rate_gyro_latest[0]));
            updated[0] = true;

            auto curr_accel =  accel_signals_generator.get_next_sample();
            accel[0] = curr_accel;
            accel[1] = 0;
            accel[2] = 0;
            // Set dafault values to vibration
            get_vibration({curr_accel,0,0}); 
            pub.msg.accelerometer_latest[0] = curr_accel;
            //pub.msg.accelerometer_latest[1] = 0;
            //pub.msg.accelerometer_latest[2] = 0
            // Other axis are redundant if we want to simulate one wave
            // Set them from 2nd element as 1st is used
            memset(pub.msg.accelerometer_latest + 1, 0, sizeof(pub.msg.accelerometer_latest)*sizeof(pub.msg.accelerometer_latest[0]));
            updated[1] = true;
            update_accel_fft();
        }
    }
    if (pub_timeout_ms != 0 && HAL_GetTick() - pub.msg.timestamp / 1000 > pub_timeout_ms) {
        if (updated[0] && updated[1]) {
            pub.msg.timestamp = HAL_GetTick() * 1000;
            pub.publish();
            // Send logging data to dronecan every second
            perform_logging_dronecan();
        }
    }
}

void ImuModule::perform_logging_dronecan (void)  {
    // Log temperature
    int16_t raw_temp{0};
    char buffer[40];
    if (imu.read_temperature (raw_temp) >= 0) {
        // Put %d instead of %f in logger to safe memory for sprintf
        snprintf(buffer, sizeof(buffer), "temperature %d",
            raw_temp_convert_to_celsius(raw_temp));
        logger.log_info(buffer);
    }
    // Log condition of MPU9250
    if (!initialized) {
        snprintf(buffer, sizeof(buffer), "MPU9250 is not initialised!");
        logger.log_error(buffer);
    } else {
        snprintf(buffer, sizeof(buffer), "MPU9250 is initialised!");
        logger.log_info(buffer);
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
    fft_gyro.update(gyro.data());
    pub.msg.rate_gyro_integral[0] = fft_gyro.dominant_frequency;
    pub.msg.rate_gyro_integral[1] = fft_gyro.dominant_mag * 1000;
    pub.msg.rate_gyro_integral[2] = fft_gyro.dominant_snr;
}
