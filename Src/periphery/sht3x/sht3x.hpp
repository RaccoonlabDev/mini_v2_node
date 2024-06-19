#ifndef SRC_PERIPHERY_SHT3X_SHT3X_HPP_
#define SRC_PERIPHERY_SHT3X_SHT3X_HPP_

#include <cstdint>

#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW 0x44
#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x45

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

struct SHT3XHandle {
    /**
     * The I2C device address.
     * @see{PCA9865_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW} and
     * @see{SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH}
     */
    uint16_t device_address;
};

class SHT3XPeriphery {
  public:
    /**
     * Execute a command defined in SHT3XCommand
     * @param handle Handle to the SHT3x device.
     * @param command SHT3XCommand
     * @return True on success, false otherwise.
     */
    static bool sendCommand(SHT3XHandle &handle, SHT3XCommand command);
    /**
     * Takes a single temperature and humidity measurement.
     * @param handle Handle to the SHT3x device.
     * @param temperature Pointer to the storage location for the sampled
     * temperature.
     * @param humidity Pointer to the storage location for the sampled humidity.
     * @return True on success, false otherwise.
     */
    static bool readTemperatureHumidity(SHT3XHandle &handle, float &temperature,
                                        float &humidity);
};

#endif // SRC_PERIPHERY_SHT3X_SHT3X_HPP_
