/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/i2c/i2c.hpp"

namespace HAL {

int8_t I2C::init() {
    return -1;
}

int8_t I2C::is_device_ready(uint16_t address, uint8_t trials) {
    (void)address;
    (void)trials;
    return -1;
}

int8_t I2C::transmit(uint16_t id, uint8_t tx[], uint8_t len) {
    (void)id;
    (void)tx;
    (void)len;
    return -1;
}

int8_t I2C::receive(uint16_t id, uint8_t *rx, uint8_t len) {
    (void)id;
    (void)rx;
    (void)len;
    return -1;
}

int32_t I2C::read_register_1_byte(uint16_t device_id, uint8_t reg_address) {
    (void)device_id;
    (void)reg_address;
    return -1;
}

int32_t I2C::read_register_2_bytes(uint16_t device_id, uint8_t reg_address) {
    (void)device_id;
    (void)reg_address;
    return -1;
}

}  // namespace HAL
