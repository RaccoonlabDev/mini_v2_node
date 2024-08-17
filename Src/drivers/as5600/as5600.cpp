/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "as5600.hpp"
#include <array>
#include "periphery/i2c/i2c.hpp"

namespace Driver {

/**
 * @note AS5600 registers
 */
static constexpr uint8_t REG_RAW_ANGLE = 0x0C;  // Unscaled and unmodified angle
static constexpr uint8_t REG_ANGLE = 0x0E;      // Scaled output value
static constexpr uint8_t REG_STATUS = 0x0B;     // Bits that indicate the current state
static constexpr uint8_t REG_MAGNITUDE = 0x1B;  // The magnitude value of the internal CORDIC

static constexpr uint8_t I2C_ADDRESS_AS5600 = 0x36;
static constexpr uint8_t I2C_AS5600 = (I2C_ADDRESS_AS5600 << 1) + 1;


int8_t AS5600::init() {
    if (auto i2c_init_status = HAL::I2C::init(); i2c_init_status < 0) {
        return i2c_init_status;
    }

    if (auto i2c_device_status = HAL::I2C::is_device_ready(I2C_AS5600); i2c_device_status < 0) {
        return i2c_device_status;
    }

    if (auto as5600_status = get_status(); as5600_status < 0) {
        return as5600_status;
    }

    return 0;
}

bool AS5600::is_ready() {
    return HAL::I2C::is_device_ready(I2C_AS5600) >= 0;
}

int16_t AS5600::get_angle() {
    auto reg_value = HAL::I2C::read_register_2_bytes(I2C_AS5600, REG_RAW_ANGLE);

    if (reg_value < 0) {
        return reg_value;
    }

    int16_t angle_deg = reg_value / 4095.0f * 360.0f;
    return std::clamp(angle_deg, (int16_t)0, (int16_t)359);
}

int16_t AS5600::get_status() {
    auto reg_value = HAL::I2C::read_register_1_byte(I2C_AS5600, REG_STATUS);
    return (reg_value >= 0) ? ((uint8_t)reg_value & 0b111000) : -reg_value;
}

int16_t AS5600::get_magnitude() {
    auto reg_value = HAL::I2C::read_register_2_bytes(I2C_AS5600, REG_MAGNITUDE);

    if (reg_value < 0) {
        return reg_value;
    }
    int16_t angle_deg = reg_value / 4095.0f * 360.0f;
    return std::clamp(angle_deg, (int16_t)0, (int16_t)359);
}

}  // namespace Driver
