/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * FIFO collaborator: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "mpu9250.hpp"

#include <cstddef>
#include <cstdio>

#include "common/logging.hpp"
#include "main.h"
#include "peripheral/spi/spi.hpp"


int8_t Mpu9250::read_accelerometer(SensorAccel* accel) const {
    std::array<std::byte, 6> buffer = {};
    auto reg = std::byte(AccelGyroRegisters::ACCEL_XOUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), 6); res < 0) {
        return res;
    }

    (*accel)[0] = (uint16_t)buffer[0] << 8 | (uint16_t)buffer[1];
    (*accel)[1] = (uint16_t)buffer[2] << 8 | (uint16_t)buffer[3];
    (*accel)[2] = (uint16_t)buffer[4] << 8 | (uint16_t)buffer[5];

    return 0;
}

int8_t Mpu9250::read_gyroscope(SensorGyro* gyro) const {
    std::array<std::byte, 6> buffer = {};
    auto reg = std::byte(AccelGyroRegisters::GYRO_XOUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), buffer.size()); res < 0) {
        return res;
    }

    (*gyro)[0] = (uint16_t)buffer[0] << 8 | (uint16_t)buffer[1];
    (*gyro)[1] = (uint16_t)buffer[2] << 8 | (uint16_t)buffer[3];
    (*gyro)[2] = (uint16_t)buffer[4] << 8 | (uint16_t)buffer[5];

    return 0;
}

int8_t Mpu9250::read_magnetometer(SensorGyro* mag) const {
    (*mag)[0] = 0.0;
    (*mag)[1] = 0.0;
    (*mag)[2] = 0.0;
    return -1;  // not supported yet
}

int8_t Mpu9250::read_temperature (int16_t& temperature) const {
    std::array<std::byte, 2> buffer = {};
    auto reg = std::byte(TemperatureRegister::TEMP_OUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), buffer.size()); res < 0) {
        return res;
    }
    temperature = static_cast<int16_t>((uint16_t)buffer[0] << 8 | (uint16_t)buffer[1]);
    return 0;
}

int8_t Mpu9250::FIFO_create () {
    // Set sample rate divider
    auto smprt_div_reg = std::byte(Mpu9250Registers::SMPLRT_DIV);
    // NOTE: setting too big value of sample RATE may cause frequent overflows
    // Consider the frequency in which FIFO is being read in your main loop
    std::byte smprt_div = std::byte(MpuSampleRate::SAMPLE_RATE_100HZ);  // 100Hz: 1000/(1+9) = 100Hz
    if (HAL::SPI::write_register(smprt_div_reg, smprt_div)) {
        return false;
    }

    // Set ACCEL_CONFIG2 for accelerometer sample rate
    auto accel_config2_reg = std::byte(Mpu9250Registers::ACCEL_CONFIG2);
    std::byte accel_config2_val{0x03};  // 44Hz filter, matches gyro
    if (HAL::SPI::write_register(accel_config2_reg, accel_config2_val)) {
        return false;
    }

    // Wait for accel config to take effect
    HAL_Delay(1);
    // Reset FIFO before configuration
    if (FIFO_reset() != 0) {
        return false;
    }

    // Wait after FIFO reset
    HAL_Delay(1);
    // Configure FIFO for accelerometer only
    FIFO_set_resolution(bitmask);

    // Wait before enabling FIFO
    HAL_Delay(1);
    // Enable FIFO
    if (FIFO_init() != 0) {
        return false;
    }

    // Wait for FIFO to stabilize
    HAL_Delay(1);
    return 0;
}

bool Mpu9250::initialize(bool useFifo = false) {
    // Reset device
    std::byte reset_pwr = std::byte(0x80);
    auto pwr_mgmt1_reg = std::byte(Mpu9250Registers::PWR_MGMT_1);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, reset_pwr)) {
        return false;
    }

    // Wait for reset to complete
    HAL_Delay(10);
    // Wake up with proper clock source
    std::byte pwr_mgmt1 = std::byte(0x01);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, pwr_mgmt1)) {
        return false;
    }

    // Wait for clock stabilization
    HAL_Delay(2);
    // Disable I2C
    std::byte disable_i2c = std::byte(0x10);
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    if (HAL::SPI::write_register(user_ctrl_reg, disable_i2c)) {
        return false;
    }

    // Wait for I2C disable to take effect
    HAL_Delay(1);
    // Set CONFIG register for DLPF and internal sample rate ***
    auto config_reg = std::byte(Mpu9250Registers::CONFIG);
    std::byte config_val{0x03};  // DLPF=44Hz, enables 1kHz internal rate
    if (HAL::SPI::write_register(config_reg, config_val)) {
        return false;
    }
    // Verify device ID
    std::byte who_am_i_value{0};
    auto reg = std::byte(Mpu9250Registers::WHO_AM_I);
    if (HAL::SPI::read_register(reg, &who_am_i_value) != 0) {
        return false;
    }
    // Create FIFO
    if (useFifo)
        if (FIFO_create())
            return false;

#ifndef USE_PLATFORM_UBUNTU
    return who_am_i_value == MPU9250_WHO_AM_I_ID;
#else
    return true;
#endif
}

int8_t Mpu9250::FIFO_set_resolution (FIFOEnableBitmask set_bitmask) {
    // No need to safe data from FIFO_ENABLE as it will be new every time
    // See docs to change value
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    if (HAL::SPI::write_register(fifo_enable_reg, std::byte(set_bitmask))) {
        return -1;
    }

    fifo_frame_bytes = __builtin_popcount(static_cast<uint8_t>(set_bitmask));
    if (static_cast<uint8_t>(std::byte(set_bitmask) & std::byte{0x08})) {
        // Check on ACCEL bit. If turned on then will be 2 more entities
        fifo_frame_bytes += 2;
    }
    // Multiply by two as one entity correspond to two bytes
    fifo_frame_bytes *= 2;
    bitmask = set_bitmask;
    return 0;
}

int8_t Mpu9250::FIFO_reset() {
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    std::byte data;

    // Disable FIFO_EN in USER_CTRL
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) return -1;
    data &= std::byte{0xBF};  // clear bit 6 (FIFO_EN)
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;

    // Disable all FIFO sources
    std::byte disable_all{0x00};
    if (HAL::SPI::write_register(fifo_enable_reg, disable_all)) return -1;

    // Wait for FIFO to settle
    HAL_Delay(1);

    // Issue FIFO_RST pulse
    std::byte rst = data | std::byte{0x04};  // set bit 2 (FIFO_RST)
    if (HAL::SPI::write_register(user_ctrl_reg, rst)) return -1;

    // Wait for reset to complete
    HAL_Delay(1);

    // Clear the FIFO_RST bit
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;

    // Wait before next operation
    HAL_Delay(1);
    return 0;
}


int8_t Mpu9250::FIFO_init () {
    std::byte enable_ctrl_FIFO = std::byte(0x40);  // 0b01000000
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    std::byte data{0};
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) {
        return -1;
    }
    enable_ctrl_FIFO |= data;  // to safe config from earlier
    if (HAL::SPI::write_register(user_ctrl_reg, enable_ctrl_FIFO)) {
        return -1;
    }
    return 0;
}

uint16_t Mpu9250::FIFO_count() {
    std::byte buf[2];
    // Read both H and L registers in one burst
    auto count_h_reg = std::byte(Mpu9250Registers::FIFO_COUNT_H);
    HAL::SPI::read_registers(count_h_reg, buf, 2);
    uint16_t high = uint8_t(buf[0]);
    uint16_t low  = uint8_t(buf[1]);
    return (high << 8) | low;
}

// __restrict keyword guarantees that none of pointers are intervined toghether and helps compiler
int8_t Mpu9250::FIFO_read(int16_t* __restrict raw_temperature,
        SensorGyro* __restrict raw_gyro, SensorAccel* __restrict raw_accel) {
    // Initialize output with safe values
    (*raw_gyro)[0] = 0; (*raw_gyro)[1] = 0; (*raw_gyro)[2] = 0;
    *raw_temperature = 0;
    (*raw_accel)[0] = 0; (*raw_accel)[1] = 0; (*raw_accel)[2] = 0;

    // Check overflow BEFORE reading count to avoid corrupted state
    std::byte int_status{0};
    auto int_status_reg = std::byte(Mpu9250Registers::INT_STATUS);
    uint16_t fifo_count = FIFO_count();

    if (HAL::SPI::read_register(int_status_reg, &int_status) != 0) {
        return -1;
    }

    std::byte fifo_overflow_mask{0x10};
    if (static_cast<uint8_t>(int_status & fifo_overflow_mask)) {
        // Reset FIFO and reconfigure
        if (FIFO_reset() != 0) {
            return -1;
        }

        // Re-enable accelerometer FIFO
        FIFO_set_resolution(bitmask);
        // Re-enable FIFO
        HAL_Delay(1);
        if (FIFO_init() != 0) {
            return -1;
        }
        return -2;  // Signal overflow handled
    }
    // Only read if we have complete frames (6 bytes per accel sample)
    if (fifo_count < fifo_frame_bytes) {
        return -3;  // Not enough data
    }

    // Read only one complete frame to avoid timing issues
    std::byte raw_data[fifo_frame_bytes];
    auto fifo_reg = std::byte(Mpu9250Registers::FIFO_R_W);
    if (HAL::SPI::read_registers(fifo_reg, raw_data, fifo_frame_bytes) != 0) {
        return -1;
    }

    // Parse accelerometer data (big-endian format)
    // TODO(ilyha_dev) : make enum for bitmask usage to improve code readability
    if (static_cast<uint8_t>(bitmask & FIFOEnableBitmask::ACCEL)){
        // Parse accelerometer data (big-endian format)
        (*raw_accel)[0] = static_cast<int16_t>(
            (uint16_t(raw_data[0]) << 8) | uint16_t(raw_data[1]));
        (*raw_accel)[1] = static_cast<int16_t>(
            (uint16_t(raw_data[2]) << 8) | uint16_t(raw_data[3]));
        (*raw_accel)[2] = static_cast<int16_t>(
            (uint16_t(raw_data[4]) << 8) | uint16_t(raw_data[5]));
    }
    if (static_cast<uint8_t>(bitmask & FIFOEnableBitmask::TEMPERATURE)) {
        *raw_temperature = static_cast<int16_t>(
            (uint16_t)raw_data[6] << 8 | (uint16_t)raw_data[7]);
    }

    if (static_cast<uint8_t>(bitmask & FIFOEnableBitmask::GYRO_X)) {
            (*raw_gyro)[0] = static_cast<int16_t>(
                (uint16_t)raw_data[8] << 8 | (uint16_t)raw_data[9]);
    }
    if (static_cast<uint8_t>(bitmask & FIFOEnableBitmask::GYRO_Y)) {
            (*raw_gyro)[1] = static_cast<int16_t>(
                (uint16_t)raw_data[10] << 8 | (uint16_t)raw_data[11]);
    }
    if (static_cast<uint8_t>(bitmask & FIFOEnableBitmask::GYRO_Z)) {
            (*raw_gyro)[2] = static_cast<int16_t>(
                (uint16_t)raw_data[12] << 8 | (uint16_t)raw_data[13]);
    }

    return 0;
}
