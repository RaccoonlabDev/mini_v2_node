/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/i2c/i2c.hpp"
#include <array>
#include "i2c.h"

namespace HAL {

static const constexpr uint32_t I2C_TIMEOUT = 30;

// The I2C instance can be overridden per board via -DNC_I2C_INSTANCE=<hi2cX>
// (e.g. am/node_h7 puts the INA228 sensors on I2C3). Defaults to hi2c1.
#if defined(NC_I2C_INSTANCE)
static I2C_HandleTypeDef* const nc_hi2c = &NC_I2C_INSTANCE;
#else
static I2C_HandleTypeDef* const nc_hi2c = &hi2c1;
#endif

int8_t I2C::init() {
    if (auto res = HAL_I2C_DeInit(nc_hi2c); res != 0) {
        return -res;
    }

    if (auto res = HAL_I2C_Init(nc_hi2c); res != 0) {
        return -res;
    }

    return 0;
}

int8_t I2C::is_device_ready(uint16_t address, uint8_t trials) {
    auto res = HAL_I2C_IsDeviceReady(nc_hi2c, address, trials, I2C_TIMEOUT);
    return (res == HAL_OK) ? 0 : -res;
}

int8_t I2C::transmit(uint16_t id, uint8_t tx[], uint8_t len) {
    auto res = HAL_I2C_Master_Transmit(nc_hi2c, id, tx, len, I2C_TIMEOUT);
    return (res == HAL_OK) ? 0 : -res;
}

int8_t I2C::receive(uint16_t id, uint8_t *rx, uint8_t len) {
    auto res = HAL_I2C_Master_Receive(nc_hi2c, id, rx, len, I2C_TIMEOUT);
    return (res == HAL_OK) ? 0 : -res;
}

int32_t I2C::read_register_1_byte(uint16_t device_id, uint8_t reg_address) {
    std::array<uint8_t, 1> tx_buffer = {{reg_address}};
    // Propagate the negative error code as documented: negating it here would make a
    // failure indistinguishable from a valid register value and defeat every caller's
    // `if (reg_value < 0)` check.
    if (auto res = HAL::I2C::transmit(device_id, tx_buffer.data(), tx_buffer.size()); res < 0) {
        return res;
    }

    std::array<uint8_t, 1> reg_value = {};
    if (auto res = HAL::I2C::receive(device_id, reg_value.data(), reg_value.size()); res < 0) {
        return res;
    }

    return reg_value[0];
}

int32_t I2C::read_register_2_bytes(uint16_t device_id, uint8_t reg_address) {
    std::array<uint8_t, 1> tx_buffer = {{reg_address}};
    // See read_register_1_byte(): the error code must stay negative.
    if (auto res = HAL::I2C::transmit(device_id, tx_buffer.data(), tx_buffer.size()); res < 0) {
        return res;
    }

    std::array<uint8_t, 2> reg_value = {};
    if (auto res = HAL::I2C::receive(device_id, reg_value.data(), reg_value.size()); res < 0) {
        return res;
    }

    return (reg_value[0] << 8) | reg_value[1];
}

}  // namespace HAL
