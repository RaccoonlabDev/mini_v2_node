/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "drivers/ina228/ina228.hpp"
#include <array>
#include "peripheral/i2c/i2c.hpp"

namespace
{

constexpr uint16_t DEVICE_ADDRESS = 0x40U << 1U;

enum Register : uint8_t {
    REG_CONFIG = 0x00U,
    REG_SHUNT_VOLTAGE = 0x04U,
    REG_BUS_VOLTAGE = 0x05U,
    REG_MANUFACTURER_ID = 0x3EU,
    REG_DEVICE_ID = 0x3FU,
};

constexpr uint16_t CONFIG_ADCRANGE_MASK = 1U << 4U;
constexpr int64_t SHUNT_VOLTAGE_LSB_UV_X1000000_RANGE_0 = 312500;
constexpr int64_t SHUNT_VOLTAGE_LSB_UV_X1000000_RANGE_1 = 78125;
constexpr int64_t BUS_VOLTAGE_LSB_UV_X10000 = 1953125;
constexpr uint16_t EXPECTED_MANUFACTURER_ID = 0x5449U;
constexpr uint16_t EXPECTED_DEVICE_ID = 0x2281U;

int32_t sign_extend_20_bit(const uint32_t value)
{
    constexpr uint32_t SIGN_BIT = 1UL << 19U;
    constexpr uint32_t SIGN_EXTENSION = 0xFFF00000UL;

    return static_cast<int32_t>((value & SIGN_BIT) != 0 ? (value | SIGN_EXTENSION) : value);
}

}  // namespace

namespace Driver
{

int8_t INA228::init()
{
    if (const auto result = HAL::I2C::init(); result != 0) {
        return result;
    }

    if (const auto result = verify_device_ready(); result != 0) {
        return result;
    }

    return verify_identity_registers();
}

int8_t INA228::read_identity(Identity* identity)
{
    if (identity == nullptr) {
        return -1;
    }

    if (const auto result = read_manufacturer_id(&identity->manufacturer_id); result != 0) {
        return result;
    }

    return read_device_id(&identity->device_id);
}

int8_t INA228::read_measurements(Measurements* measurements)
{
    if (measurements == nullptr) {
        return -1;
    }

    uint16_t config = 0;

    if (const auto result = read_config(&config); result != 0) {
        return result;
    }

    int32_t shunt_voltage_raw = 0;

    if (const auto result = read_shunt_voltage_raw(&shunt_voltage_raw); result != 0) {
        return result;
    }

    uint32_t bus_voltage_raw = 0;

    if (const auto result = read_bus_voltage_raw(&bus_voltage_raw); result != 0) {
        return result;
    }

    const int64_t shunt_voltage_lsb_uv_x1000000 =
        (config & CONFIG_ADCRANGE_MASK) != 0
        ? SHUNT_VOLTAGE_LSB_UV_X1000000_RANGE_1
        : SHUNT_VOLTAGE_LSB_UV_X1000000_RANGE_0;

    measurements->config = config;
    measurements->shunt_voltage_raw = shunt_voltage_raw;
    measurements->bus_voltage_raw = bus_voltage_raw;
    measurements->shunt_voltage_uv =
        static_cast<int32_t>(
            (static_cast<int64_t>(shunt_voltage_raw) * shunt_voltage_lsb_uv_x1000000) / 1000000);
    measurements->bus_voltage_mv =
        static_cast<uint32_t>(
            (static_cast<int64_t>(bus_voltage_raw) * BUS_VOLTAGE_LSB_UV_X10000) / 10000000);
    return 0;
}

int8_t INA228::verify_device_ready()
{
    return HAL::I2C::is_device_ready(DEVICE_ADDRESS);
}

int8_t INA228::verify_identity_registers()
{
    Identity identity{};

    if (const auto result = read_identity(&identity); result != 0) {
        return result;
    }

    if (identity.manufacturer_id != EXPECTED_MANUFACTURER_ID) {
        return -2;
    }

    if (identity.device_id != EXPECTED_DEVICE_ID) {
        return -3;
    }

    return 0;
}

int8_t INA228::read_config(uint16_t* config)
{
    return read_register_2_bytes(REG_CONFIG, config);
}

int8_t INA228::read_manufacturer_id(uint16_t* manufacturer_id)
{
    return read_register_2_bytes(REG_MANUFACTURER_ID, manufacturer_id);
}

int8_t INA228::read_device_id(uint16_t* device_id)
{
    return read_register_2_bytes(REG_DEVICE_ID, device_id);
}

int8_t INA228::read_shunt_voltage_raw(int32_t* shunt_voltage_raw)
{
    if (shunt_voltage_raw == nullptr) {
        return -1;
    }

    uint32_t reg_value = 0;

    if (const auto result = read_register_3_bytes(REG_SHUNT_VOLTAGE, &reg_value); result != 0) {
        return result;
    }

    *shunt_voltage_raw = sign_extend_20_bit(reg_value >> 4U);
    return 0;
}

int8_t INA228::read_bus_voltage_raw(uint32_t* bus_voltage_raw)
{
    if (bus_voltage_raw == nullptr) {
        return -1;
    }

    uint32_t reg_value = 0;

    if (const auto result = read_register_3_bytes(REG_BUS_VOLTAGE, &reg_value); result != 0) {
        return result;
    }

    *bus_voltage_raw = (reg_value >> 4U) & 0x000FFFFFUL;
    return 0;
}

int8_t INA228::read_register_2_bytes(uint8_t reg, uint16_t* value)
{
    if (value == nullptr) {
        return -1;
    }

    const auto reg_value = HAL::I2C::read_register_2_bytes(DEVICE_ADDRESS, reg);

    if (reg_value < 0) {
        return static_cast<int8_t>(reg_value);
    }

    *value = static_cast<uint16_t>(reg_value);
    return 0;
}

int8_t INA228::read_register_3_bytes(uint8_t reg, uint32_t* value)
{
    if (value == nullptr) {
        return -1;
    }

    std::array<uint8_t, 1> tx_buffer = {{reg}};

    const auto tx_result = HAL::I2C::transmit(DEVICE_ADDRESS, tx_buffer.data(), tx_buffer.size());
    if (tx_result < 0) {
        return tx_result;
    }

    std::array<uint8_t, 3> reg_value{};

    const auto rx_result = HAL::I2C::receive(DEVICE_ADDRESS, reg_value.data(), reg_value.size());
    if (rx_result < 0) {
        return rx_result;
    }

    *value =
        (static_cast<uint32_t>(reg_value[0]) << 16U)
        | (static_cast<uint32_t>(reg_value[1]) << 8U)
        | static_cast<uint32_t>(reg_value[2]);
    return 0;
}

}  // namespace Driver
