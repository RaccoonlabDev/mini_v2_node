#include "periphery/sht3x/sht3x.hpp"

#include <cstdint>

#include "main.h"
#include "periphery/i2c/i2c.hpp"

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb) {
    return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

static uint8_t calculate_crc(const uint8_t *data, size_t length) {
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

bool SHT3XPeriphery::sendCommand(SHT3XHandle &handle, SHT3XCommand command) {
    uint8_t command_buffer[2] = {(uint8_t)((uint16_t)command >> 8u),
                                 (uint8_t)((uint16_t)command & 0xffu)};

    return I2CPeriphery::transmit(handle.device_address << 1u, command_buffer,
                                  sizeof(command_buffer));
}

bool SHT3XPeriphery::readTemperatureHumidity(SHT3XHandle &handle,
                                             float &temperature,
                                             float &humidity) {
    sendCommand(handle, SHT3XCommand::SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH);
    HAL_Delay(1);

    uint8_t buffer[6];

    if (!I2CPeriphery::receive(handle.device_address << 1u, buffer,
                               sizeof(buffer))) {
        return false;
    }

    uint8_t temperature_crc = calculate_crc(buffer, 2);
    uint8_t humidity_crc = calculate_crc(buffer + 3, 2);
    if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
        return false;
    }

    uint16_t temperature_raw = uint8_to_uint16(buffer[0], buffer[1]);
    uint16_t humidity_raw = uint8_to_uint16(buffer[3], buffer[4]);

    temperature = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
    humidity = 100.0f * (float)humidity_raw / 65535.0f;

    return true;
}
