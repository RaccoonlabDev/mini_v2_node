/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include "sht3x.hpp"
#include "peripheral/i2c/i2c.hpp"

namespace Driver {

bool SHT3X::read(float *temperature, float *humidity) const {
    sendCommand(SHT3XCommand::SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH);

    uint8_t buffer[6];

    if (!HAL::I2C::receive(i2c_address, buffer, sizeof(buffer))) {
        return false;
    }

    if (calculate_crc(buffer, 2) != buffer[2] || calculate_crc(buffer + 3, 2) != buffer[5]) {
        return false;
    }

    uint16_t temperature_raw = uint8_to_uint16(buffer[0], buffer[1]);
    uint16_t humidity_raw = uint8_to_uint16(buffer[3], buffer[4]);

    *temperature = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
    *humidity = 100.0f * (float)humidity_raw / 65535.0f;

    return true;
}

bool SHT3X::sendCommand(SHT3XCommand command) const {
    uint8_t command_buffer[2] = {
        (uint8_t)((uint16_t)command >> 8u),
        (uint8_t)((uint16_t)command & 0xffu)
    };

    return HAL::I2C::transmit(i2c_address, command_buffer, sizeof(command_buffer));
}

uint16_t SHT3X::uint8_to_uint16(uint8_t msb, uint8_t lsb) {
    return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

uint8_t SHT3X::calculate_crc(const uint8_t *data, size_t length) {
    uint8_t crc = 0xff;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (size_t j = 0; j < 8; j++) {
            if ((crc & 0x80u) != 0) {
                crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
            } else {
                crc <<= 1u;
            }
        }
    }
    return crc;
}

}  // namespace Driver
