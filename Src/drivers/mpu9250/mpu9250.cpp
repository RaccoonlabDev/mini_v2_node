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
    FIFO_R_W = 0x74
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
    HAL_Delay(100);
    
    // Reset
    std::byte reset_pwr = std::byte(0x80);
    auto pwr_mgmt1_reg = std::byte(Mpu9250Registers::PWR_MGMT_1);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, reset_pwr)) {
        return false;
    }
    HAL_Delay(100);
    
    // Wake up with proper clock source
    std::byte pwr_mgmt1 = std::byte(0x01); // Use gyro as clock
    if (HAL::SPI::write_register(pwr_mgmt1_reg, pwr_mgmt1)) {
        return false;
    }
    
    // Sample rate divider
    std::byte smprt_div = std::byte{0x09}; // 100Hz
    HAL::SPI::write_register(std::byte{0x19}, smprt_div);
    
    // Disable I2C
    std::byte disable_i2c = std::byte(0x10);
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    if (HAL::SPI::write_register(user_ctrl_reg, disable_i2c)) {
        return false;
    }
    
    FIFO_reset();
    // Configure FIFO
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    
    if (HAL::SPI::write_register(fifo_enable_reg, std::byte{0x08})) {
        return -1;
    }
    fifo_entities = 3;
    FIFO_init();
    HAL_Delay(10);
    
    // Verify device
    std::byte who_am_i_value{0};
    auto reg = std::byte(Mpu9250Registers::WHO_AM_I);
    HAL::SPI::read_register(reg, &who_am_i_value);
    
    #ifndef USE_PLATFORM_UBUNTU
        return who_am_i_value == MPU9250_WHO_AM_I_ID;
    #else
        return true;
    #endif
}


int8_t Mpu9250::FIFO_reset () {
    auto user_ctrl_reg  = std::byte(Mpu9250Registers::USER_CTRL);
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    std::byte data;

    // 1. Disable FIFO_EN in USER_CTRL
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) return -1;
    data &= std::byte{0xBF};            // clear bit 6
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;

    // 2. Disable all sources in FIFO_ENABLE
    if (HAL::SPI::write_register(fifo_enable_reg, std::byte{0x00})) return -1;

    // 3. Issue FIFO_RST pulse
    std::byte rst = data | std::byte{0x04};  // set bit 2
    if (HAL::SPI::write_register(user_ctrl_reg, rst)) return -1;

    HAL_Delay(1);  // allow reset to propagate

    // 4. Clear the FIFO_RST bit
    if (HAL::SPI::write_register(user_ctrl_reg, data)) return -1;

    // 5. Delay before reading count
    HAL_Delay(1);

    char msg[30];
    Logging logger{"FIFO"};
    snprintf(msg, sizeof(msg), "HELL NAH RESET %d", (int)FIFO_count());
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
    HAL::SPI::read_registers(std::byte{0x72}, buf, 2);
    uint16_t high = uint8_t(buf[0]);
    uint16_t low  = uint8_t(buf[1]);
    return (high << 8) | low;
}


int8_t Mpu9250::FIFO_read(int16_t* raw_temperature, std::array<int16_t, 3>* raw_gyro, 
                          std::array<int16_t, 3>* raw_accel) {
    Logging logger{"MY_FIFO"};
    (*raw_gyro)[0] = 123;
    *raw_temperature = 12;
    //uint16_t fifo_count = FIFO_count();

    
    // Check for overflow
    std::byte int_status{0};
    auto int_status_reg = std::byte(Mpu9250Registers::INT_STATUS);
    HAL::SPI::read_register(int_status_reg, &int_status);
    //std::byte fifo_overflow_int_mask{0x10};
    
    /*if (static_cast<uint8_t>(int_status & fifo_overflow_int_mask)) {
        char msg[35];
        snprintf(msg, sizeof(msg), "Overflow detected! count: %d", fifo_count);
        logger.log_error(msg);
        
        HAL::SPI::read_register(int_status_reg, &int_status);
        
        // Also clear the FIFO
        if (fifo_count > 0) {
            std::byte raw_data[fifo_count];
            auto reg = std::byte(Mpu9250Registers::FIFO_R_W);
            HAL::SPI::read_registers(reg, raw_data, fifo_count);
            logger.log_error("FIFO cleaned!");
        }
        FIFO_reset();
        auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
        if (HAL::SPI::write_register(fifo_enable_reg, std::byte{0x08})) {
            return -1;
        }
        return -1;
    }*/

    // Only read if we have complete frames
    //size_t bytes_per_frame = fifo_entities * 2;
    // How many full frames are waiting?


    //if (fifo_count >= bytes_per_frame) {
        // Read only one complete frame
        std::byte raw_data[6];
        auto reg = std::byte(Mpu9250Registers::FIFO_R_W);
        HAL::SPI::read_registers(reg, raw_data, 6);
        
        // Parse accelerometer data
        //const size_t offset = (num_frames - 1) * bytes_per_frame;
        for (int axis = 0; axis < 3; ++axis) {
        (*raw_accel)[axis] =
            (uint16_t(raw_data[2*axis]) << 8) |
             uint16_t(raw_data[2*axis + 1]);
        }
    //}
        
        return 0;
    //}
    
    //return -2; // Not enough data
}
