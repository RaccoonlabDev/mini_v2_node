/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "mpu9250.hpp"
#include <cstddef>
#include <cstdio>
#include "peripheral/spi/spi.hpp"
#include <logging.hpp>
#include "main.h"
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

int8_t Mpu9250::FIFO_reset () const {
    std::byte reset_ctrl_FIFO = std::byte(0x04); // 0b00000100
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    std::byte data{0};
    if (HAL::SPI::read_register(user_ctrl_reg, &data)) {
        return -1;
    }
    reset_ctrl_FIFO |= data; // to safe config from earlier
    reset_ctrl_FIFO &= std::byte{0xBF}; // is 0b10111111 to remove FIFO_EN
    if (HAL::SPI::write_register(user_ctrl_reg, reset_ctrl_FIFO)) {
        return -1;
    }
    HAL_Delay(100); // give time to reset
    if (HAL::SPI::write_register(user_ctrl_reg, data)) {
        return -1;
    }
    return 0;
}

int8_t Mpu9250::FIFO_set_resolution (std::byte set_bitmask) {
    FIFO_reset();
    // No need to safe data from FIFO_ENABLE as it will be new every time
    // See docs to change value
    auto fifo_enable_reg = std::byte(Mpu9250Registers::FIFO_ENABLE);
    if (HAL::SPI::write_register(fifo_enable_reg, set_bitmask)) {
        return -1;
    }
    fifo_entities = __builtin_popcount(static_cast<uint8_t>(bitmask));
    if (static_cast<uint8_t>(bitmask & std::byte{0x08})) {
        // Check on ACCEL byte. If turned on then will be 2 more entities
        fifo_entities += 2;
    }
    bitmask = set_bitmask;
    return 0;
}

int8_t Mpu9250::FIFO_init () {
    FIFO_reset();
    FIFO_set_resolution (bitmask);
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

int8_t FIFO_get_count (uint16_t& count) {
    static Logging logger("Mpu9250::read_fifo_count");
    std::array<std::byte, 2> byte_count = {};
    auto reg = std::byte(Mpu9250Registers::FIFO_COUNT_H);
    if (auto res = HAL::SPI::read_registers(reg, byte_count.data(), 2); res < 0) {
        char msg[20];
        snprintf(msg, sizeof(msg), "fifo_count: %d", res);
        logger.log_error(msg);
        return res;
    }
    count = (uint16_t)byte_count[0] << 8 | (uint16_t)byte_count[1];
    return 0;
}


int8_t Mpu9250::FIFO_read (int16_t* raw_temperature, int16_t* raw_gyro, int16_t* raw_accel) const {
    // I didn't use std::array here due to the dynamically changing fifo_entities
    // Multiply by two as on each entity we need two bytes (Low and High registers)
    std::byte raw_data[fifo_entities * 2];
    // First check on overflow
    std::byte int_status {0};
    auto int_status_reg = std::byte(Mpu9250Registers::INT_STATUS);
    HAL::SPI::read_register(int_status_reg, &int_status);
    std::byte fifo_overflow_int_mask {0x10};
    if (static_cast<uint8_t>(int_status & fifo_overflow_int_mask)) {
        // if true then overflow happened
        FIFO_reset();
        return -1;
    }
    // Then get amount of bytes ready to be read in fifo
    // Note: fifo count indicates = number of written bytes in fifo
    uint16_t fifo_count;
    FIFO_get_count(fifo_count);
    // Multiply by two as one entity is two bytes
    // From docs MPU9250 has 512 bytes in FIFO at max
    while (fifo_count >= fifo_entities * 2 && fifo_count < 512) {
        // Get all data from FIFO
        auto reg = std::byte(Mpu9250Registers::FIFO_R_W);
        HAL::SPI::read_registers(reg, raw_data, fifo_entities * 2);
        FIFO_get_count(fifo_count);
    }
    // (bitmask >> 4) & std::byte{0b0111} to take only gyro part of data from bitmask
    uint8_t gyro_size =  __builtin_popcount(static_cast<uint8_t>((bitmask >> 4) & std::byte{0b0111}));
    for (int i = 7; i > 3; i --) {
        if (static_cast<uint8_t>(std::byte{0b1} & (bitmask >> i))) {
            if (i == 7) {
                *raw_temperature = static_cast<int16_t>(raw_data[6] << 8 | raw_data[7]);
            } else if (3 < i && i < 7) {
                // 6 as gyro_x starts at 6th i
                // i == 4 -> 12, 13
                // i == 5 -> 10, 11
                // i == 6 -> 8, 9
                // Solution of linear dependency: x = -0.5, b = 10
                raw_gyro[std::min(6 - i, gyro_size-1)] = static_cast<int16_t>(raw_data[20 - 2 * i] << 8 | raw_data[(20 - 2 * i) + 1]);
            } else {
                for (int j = 0; j < 3; j++) {
                    raw_accel[j] = static_cast<int16_t>(raw_data[j] << 8 | raw_data[j+1]);
                }
            }
        }
    }

    return 0;


}

bool Mpu9250::initialize() {
    // Power-on delay
    // This delay is main problem solver for a case
    // when MPU returned error on initialisation if just reconnect CAN to node
    HAL_Delay(100);
    // Disable I2C interface to enable SPI mode
    std::byte disable_i2c = std::byte(0x10);  // I2C_IF_DIS bit
    auto user_ctrl_reg = std::byte(Mpu9250Registers::USER_CTRL);
    if (HAL::SPI::write_register(user_ctrl_reg, disable_i2c)) {
        return false;
    }
    // Reset power management 1 register
    std::byte reset_pwr = std::byte(0x80);  // DEVICE_RESET bit
    auto pwr_mgmt1_reg = std::byte(Mpu9250Registers::PWR_MGMT_1);
    if (HAL::SPI::write_register(pwr_mgmt1_reg, reset_pwr)) {
        return false;
    }
    HAL_Delay(100);  // Wait for reset to complete
    
    FIFO_reset();
    FIFO_init();

    // Only after necessary steps check who am i
    std::byte who_am_i_value{0};
    auto reg = std::byte(Mpu9250Registers::WHO_AM_I);
    HAL::SPI::read_register(reg, &who_am_i_value);
    // Assume that MPU is always initialised in Ubuntu
    #ifndef USE_PLATFORM_UBUNTU
        return who_am_i_value == MPU9250_WHO_AM_I_ID;
    #elif USE_PLATFORM_UBUNTU == 1
        return true;
    #endif
}

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

