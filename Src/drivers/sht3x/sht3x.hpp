/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_DRIVERS_SHT3X_SHT3X_HPP_
#define SRC_DRIVERS_SHT3X_SHT3X_HPP_

#include <cstdint>
#include <cstddef>

namespace Driver {

enum class SHT3XCommand {
    SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06,
    SHT3X_COMMAND_CLEAR_STATUS = 0x3041,
    SHT3X_COMMAND_SOFT_RESET = 0x30A2,
    SHT3X_COMMAND_HEATER_ENABLE = 0x306d,
    SHT3X_COMMAND_HEATER_DISABLE = 0x3066,
    SHT3X_COMMAND_READ_STATUS = 0xf32d,
    SHT3X_COMMAND_FETCH_DATA = 0xe000,
    SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737,
    SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272a
};


class SHT3X {
public:
    static constexpr uint8_t DEV_ADDR_PIN_LOW = 0x44;
    static constexpr uint8_t DEV_ADDR_PIN_HIGH = 0x45;

    SHT3X(uint8_t dev_addr): device_address(dev_addr) {}

    /**
     * @brief Takes a single temperature and humidity measurement.
     * @param temperature Pointer to the storage location for the sampled temperature.
     * @param humidity Pointer to the storage location for the sampled humidity.
     * @return True on success, false otherwise.
     */
    bool read(float* temperature, float* humidity) const;

private:
    /**
     * @brief Execute a command defined in SHT3XCommand
     * @param handle Handle to the SHT3x device.
     * @param command SHT3XCommand
     * @return True on success, false otherwise.
     */
    static bool sendCommand(uint8_t device_address, SHT3XCommand command);

    static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb);
    static uint8_t calculate_crc(const uint8_t* data, size_t length);

    uint8_t device_address;
};

}  // namespace Driver

#endif  // SRC_DRIVERS_SHT3X_SHT3X_HPP_
