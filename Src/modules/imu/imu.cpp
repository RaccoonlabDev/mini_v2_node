/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * New readings collaborator: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include <math.h>
#include <concepts>
#include "imu.hpp"
#include "params.hpp"
#include "common/logging.hpp"

REGISTER_MODULE(ImuModule)

// Helper functions
template <typename Enum>
constexpr auto to_underlying(Enum value) -> std::underlying_type_t<Enum> {
    static_assert(std::is_enum_v<Enum>, "Enum must be an enum type");
    return static_cast<std::underlying_type_t<Enum>>(value);
}

template <typename Enum>
constexpr bool has_bit(Enum bitmask, Enum bit) {
    static_assert(std::is_enum_v<Enum>, "Enum must be an enum type");
    return (to_underlying(bitmask) & to_underlying(bit)) != 0;
}

void ImuModule::init() {
    set_initialize(imu.initialize(is_fifo_created));
    set_mode(Mode::STANDBY);
    // Module frequency must be the same as FFT frequency for correct FFT work
    fft_accel.init(WINDOW_SIZE, NUM_AXES, MODULE_FREQ_HZ);
    fft_accel.fft_min_freq = FFT_MIN_FREQ;
    fft_gyro.init(WINDOW_SIZE, NUM_AXES, MODULE_FREQ_HZ);
    fft_gyro.fft_min_freq = FFT_MIN_FREQ;
}

void ImuModule::set_initialize (bool new_initialized) {
    char buffer[40];
    if (!new_initialized) {
        snprintf(buffer, sizeof(buffer), "MPU9250 is not initialised!");
        logger.log_error(buffer);
    } else {
        snprintf(buffer, sizeof(buffer), "MPU9250 is initialised!");
        logger.log_info(buffer);
    }
    this->initialized = new_initialized;
}

void ImuModule::update_params() {
    auto pub_frequency = static_cast<uint16_t>(
                                paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_PUB_FREQUENCY));
    pub_timeout_ms = pub_frequency == 0 ? 0 : 1000 / pub_frequency;
    publisher_bitmask = static_cast<Publisher_bitmask>
        (paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_PUBLISHERS_BITMASK));

    data_source = static_cast<Data_source>
        (paramsGetIntegerValue(IntParamsIndexes::PARAM_IMU_DATA_SOURCE));

    set_health((publisher_bitmask == Publisher_bitmask::DISABLED || initialized) ?
        Module::Status::OK : Module::Status::MAJOR_FAILURE);
    gen_amplitude = static_cast<uint16_t>
        (paramsGetIntegerValue(IntParamsIndexes::SYNTHETIC_AMPLITUDE));
    gen_freq = static_cast<uint16_t>(paramsGetIntegerValue(IntParamsIndexes::SYNTHETIC_FREQ_GEN));
    if (publisher_bitmask != Publisher_bitmask::DISABLED) {
        set_mode(initialized ? Mode::STANDBY : Mode::INITIALIZATION);
    }
}


void ImuModule::spin_once() {
    bool isFifoReinitBroken = false;
    // In those cases spin_once meaningless
    if (data_source == Data_source::DISABLED || publisher_bitmask == Publisher_bitmask::DISABLED || pub_timeout_ms == 0) {
            return;
    }

    std::array<bool, 2> updated {false, false};

    if (initialized && data_source == Data_source::ENABLE_REG_READINGS) {
        process_real_register(updated);
        if (fifo_state) {
            imu.FIFO_reset();
            fifo_state = false;
        }
    }
    if (initialized && data_source == Data_source::ENABLE_FIFO_READINGS) {
        // Need to check if fifo was created in first place as in creation
        // essential sample rate initialised
        if (!fifo_state && is_fifo_created) {
            if (imu.FIFO_init() && imu.FIFO_set_resolution(FIFOEnableBitmask::ENABLE_ALL)) {
                fifo_state = true;
                isFifoReinitBroken = false;
            } else {
                // set error in fifo reinit
                isFifoReinitBroken = true;
            }
        }
        process_real_fifo(updated);
    }
    if (data_source == Data_source::ENABLE_SYNTH_GEN){
        process_random_gen(updated);
        if (fifo_state) {
            imu.FIFO_reset();
            fifo_state = false;
        }
    }

    // Publish message
    if ((initialized || data_source == Data_source::ENABLE_SYNTH_GEN) &&
            HAL_GetTick() - pub.msg.timestamp / 1000 > pub_timeout_ms){
        if (updated[0] && updated[1]) {
            pub.publish();
            pub.msg.timestamp = HAL_GetTick() * 1000;
        }
    }
    // Publish normal logs
    static uint64_t log_timestamp = 0;
    if (!initialized && (HAL_GetTick() - log_timestamp / 1000 > log_timeout_ms)) {
        logger.log_warn("IMU is not initialised only synthetic read is possible");
        log_timestamp = HAL_GetTick() * 1000;
        if (isFifoReinitBroken) {
            logger.log_error("FIFO reinitialisation failed!");
        }
    }
}

void ImuModule::get_vibration(std::array<float, 3> data) {
    if (!has_bit(publisher_bitmask, Publisher_bitmask::ENABLE_VIB_ESTIM)) {
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
    if (!has_bit(publisher_bitmask, Publisher_bitmask::ENABLE_FFT_ACC)) {
        return;
    }
    fft_accel.update(accel.data());
    pub.msg.accelerometer_integral[0] = fft_accel.dominant_frequency;
    pub.msg.accelerometer_integral[1] = fft_accel.dominant_mag * 1000;
    pub.msg.accelerometer_integral[2] = fft_accel.dominant_snr;
}

void ImuModule::update_gyro_fft() {
    if (!has_bit(publisher_bitmask, Publisher_bitmask::ENABLE_FFT_GYR)) {
        return;
    }
    fft_gyro.update(gyro.data());
    pub.msg.rate_gyro_integral[0] = fft_gyro.dominant_frequency;
    pub.msg.rate_gyro_integral[1] = fft_gyro.dominant_mag * 1000;
    pub.msg.rate_gyro_integral[2] = fft_gyro.dominant_snr;
}


// Process reading functions

void ImuModule::process_random_gen (std::array<bool, 2>& updated){
    // Set values for to generate
    accel_signal_generator.setAmpl(gen_amplitude);
    accel_signal_generator.setFreq(gen_freq);
    memset(pub.msg.rate_gyro_latest, 0,
        sizeof(pub.msg.rate_gyro_latest));
    updated[0] = true;

    auto curr_accel =  accel_signal_generator.get_next_sample();
    accel[0] = curr_accel;
    accel[1] = 0;
    accel[2] = 0;
    // Set dafault values to vibration
    get_vibration({0, 0, 0});
    pub.msg.accelerometer_latest[0] = curr_accel;
    // Other axis are redundant if we want to simulate one wave
    // Set them from 2nd element as 1st is used
    memset(pub.msg.accelerometer_latest + 1, 0,
        sizeof(pub.msg.accelerometer_latest) - sizeof(pub.msg.accelerometer_latest[0]));
    updated[1] = true;
    update_accel_fft();
}

void ImuModule::process_real_fifo (std::array<bool, 2>& updated){
    static uint32_t last_read = 0;
    uint32_t current_time = HAL_GetTick();

    if (current_time - last_read >= FIFO_READING_RATE_MS) {
        std::array<int16_t, NUM_AXES> mag_raw;
        if (imu.read_magnetometer(&mag_raw) >= 0) {
            mag.publish();
        }
        std::array<int16_t, 3> accel_raw = {0, 0, 0};
        std::array<int16_t, 3> gyro_raw = {0, 0, 0};
        int16_t temp_raw;

        int8_t result = imu.FIFO_read(&temp_raw, &gyro_raw, &accel_raw);
        if (result == 0) {
            // Success - process data
            gyro = {
                raw_gyro_to_rad_per_second(gyro_raw[0]),
                raw_gyro_to_rad_per_second(gyro_raw[1]),
                raw_gyro_to_rad_per_second(gyro_raw[2])};
            pub.msg.rate_gyro_latest[0] = gyro[0];
            pub.msg.rate_gyro_latest[1] = gyro[1];
            pub.msg.rate_gyro_latest[2] = gyro[2];
            updated[0] = true;
            accel = {
                    raw_accel_to_meter_per_square_second(accel_raw[0]),
                    raw_accel_to_meter_per_square_second(accel_raw[1]),
                    raw_accel_to_meter_per_square_second(accel_raw[2])};
            get_vibration(accel);
            pub.msg.accelerometer_latest[0] = accel[0];
            pub.msg.accelerometer_latest[1] = accel[1];
            pub.msg.accelerometer_latest[2] = accel[2];
            updated[1] = true;
        } else if (result == -2) {
            // Overflow handled, continue without updating
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "FIFO overflow handled. Improve timings?");
            logger.log_warn(buffer);

        } else if (result == -3) {
            // Not enough data, normal condition
            // Don't log this as error

        } else {
            // Error condition
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "FIFO read error: %d", result);
            logger.log_error(buffer);
        }

        last_read = current_time;
    }
    // Important note: I moved fft outside of the FIFO redings,
    // So fifo read rate won't mess up with FFT timings
    update_gyro_fft();
    update_accel_fft();
}

void ImuModule::process_real_register (std::array<bool, 2>& updated) {
    // Usual reading routine
    std::array<int16_t, 3> mag_raw;
    if (imu.read_magnetometer(&mag_raw) >= 0) {
        mag.publish();
    }

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
}
