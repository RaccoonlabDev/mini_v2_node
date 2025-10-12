/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "mpu9250.hpp"
#include <cstddef>
#include <cstdio>
#include "peripheral/spi/spi.hpp"
#include "common/logging.hpp"
#include "main.h"
#include <string>
// Register Map for Gyroscope and Accelerometer
enum class AccelGyroRegisters : uint8_t {
    ACCEL_XOUT_H = 0x3B,
    GYRO_XOUT_H = 0x43    
};

enum class TemperatureRegister : uint8_t {
    TEMP_OUT_H = 0x41
};
enum class Mpu9250Registers : uint8_t {
    USER_CTRL = 0x6A,
    PWR_MGMT_1 = 0x6B,
    WHO_AM_I = 0x75,
    FIFO_ENABLE = 0x23,
    INT_STATUS = 0x3A,
    FIFO_COUNT_H = 0x72,
    FIFO_R_W = 0x74,
    CONFIG = 0x1A,
    SMPLRT_DIV = 0x19,
    ACCEL_CONFIG2 = 0x1D
};

// Register Map for Magnetometer (AK8963)
enum class MagnetometerResgiter : uint8_t {
    REG_MAG_XOUT_L = 0x03,
};

constexpr auto MPU9250_WHO_AM_I_ID = std::byte(0x71);   // REG_WHO_AM_I expected value




int8_t Mpu9250::read_accelerometer(std::array<int16_t, 3>* accel) const {
    static Logging logger("Mpu9250::read_accelerometer");
    std::array<std::byte, 6> buffer = {};
    auto reg = std::byte(AccelGyroRegisters::ACCEL_XOUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), 6); res < 0) {
        char msg[10];
        snprintf(msg, sizeof(msg), "acc: %d", res);
        logger.log_error(msg);
        return res;
    }

    (*accel)[0] = (uint16_t)buffer[0] << 8 | (uint16_t)buffer[1];
    (*accel)[1] = (uint16_t)buffer[2] << 8 | (uint16_t)buffer[3];
    (*accel)[2] = (uint16_t)buffer[4] << 8 | (uint16_t)buffer[5];

    return 0;
}

int8_t Mpu9250::read_gyroscope(std::array<int16_t, 3>* gyro) const {
    static Logging logger("Mpu9250::read_gyroscope");
    std::array<std::byte, 6> buffer = {};
    auto reg = std::byte(AccelGyroRegisters::GYRO_XOUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), buffer.size()); res < 0) {
        char msg[10];
        snprintf(msg, sizeof(msg), "gyr: %d", res);
        logger.log_error(msg);
        return res;
    }

    (*gyro)[0] = (uint16_t)buffer[0] << 8 | (uint16_t)buffer[1];
    (*gyro)[1] = (uint16_t)buffer[2] << 8 | (uint16_t)buffer[3];
    (*gyro)[2] = (uint16_t)buffer[4] << 8 | (uint16_t)buffer[5];

    return 0;
}

int8_t Mpu9250::read_magnetometer(std::array<int16_t, 3>* mag) const {
    (*mag)[0] = 0.0;
    (*mag)[1] = 0.0;
    (*mag)[2] = 0.0;
    return -1;  // not supported yet
}

int8_t Mpu9250::read_temperature (int16_t& temperature) const {
    static Logging logger("Mpu9250::temperature");
    std::array<std::byte, 2> buffer = {};
    auto reg = std::byte(TemperatureRegister::TEMP_OUT_H);
    if (auto res = HAL::SPI::read_registers(reg, buffer.data(), buffer.size()); res < 0) {
        char msg[12];
        snprintf(msg, sizeof(msg), "temp: %d", res);
        logger.log_error(msg);
        return res;
    }
    temperature = static_cast<int16_t>((uint16_t)buffer[0] << 8 | (uint16_t)buffer[1]);
    return 0;
}

bool Mpu9250::initialize() {
    // Reset device
    std::byte reset_pwr = std::byte(0x80);
    auto pwr_mgmt1_reg = std::byte(Mpu9250Registers::PWR_MGMT_1);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, reset_pwr)) {
        return false;
    }
    
    // Wait for reset to complete (critical!)
    for(volatile int i = 0; i < 100000; i++); // ~10ms delay
    
    // Wake up with proper clock source
    std::byte pwr_mgmt1 = std::byte(0x01);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, pwr_mgmt1)) {
        return false;
    }
    
    // Wait for clock stabilization
    for(volatile int i = 0; i < 20000; i++); // ~2ms delay
    
    // Disable I2C (critical for SPI mode)
    std::byte disable_i2c = std::byte(0x10);
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    if (HAL::SPI::write_register(user_ctrl_reg, disable_i2c)) {
        return false;
    }
    
    // Wait for I2C disable to take effect
    for(volatile int i = 0; i < 10000; i++); // ~1ms delay
    
    // *** CRITICAL: Set CONFIG register for DLPF and internal sample rate ***
    auto config_reg = std::byte(Mpu9250Registers::CONFIG);
    std::byte config_val{0x03};  // DLPF=44Hz, enables 1kHz internal rate
    if (HAL::SPI::write_register(config_reg, config_val)) {
        return false;
    }
    
    // Wait for CONFIG to take effect
    for(volatile int i = 0; i < 5000; i++); // ~500us delay
    
    // *** Now SMPLRT_DIV will work! Set sample rate divider ***
    auto smprt_div_reg = std::byte(Mpu9250Registers::SMPLRT_DIV);
    std::byte smprt_div = std::byte{0x04}; // 200Hz: 1000/(1+4) = 200Hz
    if (HAL::SPI::write_register(smprt_div_reg, smprt_div)) {
        return false;
    }
    
    // *** CRITICAL: Set ACCEL_CONFIG2 for accelerometer sample rate ***
    auto accel_config2_reg = std::byte(Mpu9250Registers::ACCEL_CONFIG2);
    std::byte accel_config2_val{0x03};  // 44Hz filter, matches gyro
    if (HAL::SPI::write_register(accel_config2_reg, accel_config2_val)) {
        return false;
    }
    
    // Wait for accel config to take effect  
    for(volatile int i = 0; i < 5000; i++); // ~500us delay
    
    // Reset FIFO before configuration
    if (FIFO_reset() != 0) {
        return false;
    }
    
    // Wait after FIFO reset
    for(volatile int i = 0; i < 5000; i++); // ~500us delay
    
    // Configure FIFO for accelerometer only
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    std::byte accel_bitmask{0x08};
    if (HAL::SPI::write_register(fifo_enable_reg, accel_bitmask)) {
        return false;
    }
    
    fifo_entities = 3; // 3 axes for accelerometer
    
    // Wait before enabling FIFO
    for(volatile int i = 0; i < 5000; i++); // ~500us delay
    
    // Enable FIFO
    if (FIFO_init() != 0) {
        return false;
    }
    
    // Wait for FIFO to stabilize
    for(volatile int i = 0; i < 5000; i++); // ~500us delay
    
    // Verify device ID
    std::byte who_am_i_value{0};
    auto reg = std::byte(Mpu9250Registers::WHO_AM_I);
    if (HAL::SPI::read_register(reg, &who_am_i_value) != 0) {
        return false;
    }
    
#ifndef USE_PLATFORM_UBUNTU
    return who_am_i_value == MPU9250_WHO_AM_I_ID;
#else
    return true;
#endif
}

int8_t Mpu9250::FIFO_reset() {
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    std::byte data;

    // 1. Disable FIFO_EN in USER_CTRL
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) return -1;
    data &= std::byte{0xBF}; // clear bit 6 (FIFO_EN)
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;
    
    // 2. Disable all FIFO sources
    std::byte disable_all{0x00};
    if (HAL::SPI::write_register(fifo_enable_reg, disable_all)) return -1;
    
    // 3. Wait for FIFO to settle
    for(volatile int i = 0; i < 1000; i++); // ~100us
    
    // 4. Issue FIFO_RST pulse
    std::byte rst = data | std::byte{0x04}; // set bit 2 (FIFO_RST)
    if (HAL::SPI::write_register(user_ctrl_reg, rst)) return -1;
    
    // 5. Wait for reset to complete
    for(volatile int i = 0; i < 1000; i++); // ~100us
    
    // 6. Clear the FIFO_RST bit
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;
    
    // 7. Wait before next operation
    for(volatile int i = 0; i < 1000; i++); // ~100us
    
    // 8. Verify FIFO count is zero
    uint16_t count = FIFO_count();
    if (count != 0) {
        char msg[45];
        Logging logger{"FIFO"};
        snprintf(msg, sizeof(msg), "FIFO reset failed, count: %d", count);
        logger.log_error(msg);
        return -1;
    }
    
    char msg[30];
    Logging logger{"FIFO"};
    snprintf(msg, sizeof(msg), "FIFO reset successful");
    logger.log_error(msg);
    return 0;
}


int8_t Mpu9250::FIFO_init () {
    std::byte enable_ctrl_FIFO = std::byte(0x40); // 0b01000000 
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    std::byte data{0};
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) {
        return -1;
    }
    enable_ctrl_FIFO |= data; // to safe config from earlier
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
    char msg[30];
    Logging logger{"MY_FIFO"};
    snprintf(msg, sizeof(msg), "FIFO COUNT h %d, l %d", (int)high, (int)low);
    logger.log_error(msg);
    return (high << 8) | low;
}


int8_t Mpu9250::FIFO_read(int16_t* raw_temperature, std::array<int16_t, 3>* raw_gyro,
                          std::array<int16_t, 3>* raw_accel) {
    
    Logging logger{"FIFO_READ"};
    char msg[38];

    // Initialize output with safe values
    (*raw_gyro)[0] = 0; (*raw_gyro)[1] = 0; (*raw_gyro)[2] = 0;
    *raw_temperature = 0;
    (*raw_accel)[0] = 0; (*raw_accel)[1] = 0; (*raw_accel)[2] = 0;
    
    // Check overflow BEFORE reading count to avoid corrupted state
    std::byte int_status{0};
    auto int_status_reg = std::byte(Mpu9250Registers::INT_STATUS);
    uint16_t fifo_count = FIFO_count();
    
    snprintf(msg, sizeof(msg), "FIFO count: %d", (int)fifo_count);
    logger.log_error(msg);
    if (HAL::SPI::read_register(int_status_reg, &int_status) != 0) {
        logger.log_error("Failed to read INT_STATUS");
        return -1;
    }
    
    std::byte fifo_overflow_mask{0x10};
    if (static_cast<uint8_t>(int_status & fifo_overflow_mask)) {
        
        // Reset FIFO and reconfigure
        if (FIFO_reset() != 0) {
            return -1;
        }
        snprintf(msg, sizeof(msg), "FIFO overflow detected, reseted");
        logger.log_error(msg);

        // Re-enable accelerometer FIFO
        auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
        std::byte accel_bitmask{0x08};
        for(volatile int i = 0; i < 1000; i++);
        if (HAL::SPI::write_register(fifo_enable_reg, accel_bitmask)) {
            return -1;
        }
        snprintf(msg, sizeof(msg), "FIFO overflow detected, configured");
        logger.log_error(msg);
        // Re-enable FIFO
        for(volatile int i = 0; i < 1000; i++);
        if (FIFO_init() != 0) {
            
            return -1;
        }
        
        snprintf(msg, sizeof(msg), "FIFO overflow detected, reinit");
        logger.log_error(msg);
        return -2; // Signal overflow handled
    }
    
    // Get FIFO count
    
    
    // Only read if we have complete frames (6 bytes per accel sample)
    if (fifo_count < 6) {
        return -3; // Not enough data
    }
    
    // Read only one complete frame to avoid timing issues
    std::byte raw_data[6];
    auto fifo_reg = std::byte(Mpu9250Registers::FIFO_R_W);
    if (HAL::SPI::read_registers(fifo_reg, raw_data, 6) != 0) {
        logger.log_error("Failed to read FIFO data");
        return -1;
    }
    
    // Parse accelerometer data (big-endian format)
    (*raw_accel)[0] = (int16_t)((uint16_t(raw_data[0]) << 8) | uint16_t(raw_data[1]));
    (*raw_accel)[1] = (int16_t)((uint16_t(raw_data[2]) << 8) | uint16_t(raw_data[3]));
    (*raw_accel)[2] = (int16_t)((uint16_t(raw_data[4]) << 8) | uint16_t(raw_data[5]));
    
    snprintf(msg, sizeof(msg), "Accel: %d, %d, %d", 
             (int)(*raw_accel)[0], (int)(*raw_accel)[1], (int)(*raw_accel)[2]);
    logger.log_info(msg);
    
    return 0;
}