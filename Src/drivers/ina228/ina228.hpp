/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRIVERS_INA228_INA228_HPP_
#define SRC_DRIVERS_INA228_INA228_HPP_

#include <cstdint>

namespace Driver
{

class INA228
{
public:
    struct Identity {
        uint16_t manufacturer_id{0};
        uint16_t device_id{0};
    };

    struct Measurements {
        uint16_t config{0};
        int32_t shunt_voltage_raw{0};
        uint32_t bus_voltage_raw{0};
        int32_t shunt_voltage_uv{0};
        uint32_t bus_voltage_mv{0};
    };

    /**
     * @brief Initializes I2C and verifies that the INA228 answers on the bus.
     * @return 0 on success, otherwise a negative error code.
     */
    static int8_t init();

    /**
     * @brief Reads the INA228 identity registers.
     * @return 0 on success, otherwise a negative error code.
     */
    static int8_t read_identity(Identity* identity);

    /**
     * @brief Reads the current bus and shunt voltage measurements.
     * @return 0 on success, otherwise a negative error code.
     */
    static int8_t read_measurements(Measurements* measurements);

private:
    static int8_t verify_device_ready();
    static int8_t verify_identity_registers();
    static int8_t read_config(uint16_t* config);
    static int8_t read_manufacturer_id(uint16_t* manufacturer_id);
    static int8_t read_device_id(uint16_t* device_id);
    static int8_t read_shunt_voltage_raw(int32_t* shunt_voltage_raw);
    static int8_t read_bus_voltage_raw(uint32_t* bus_voltage_raw);
    static int8_t read_register_2_bytes(uint8_t reg, uint16_t* value);
    static int8_t read_register_3_bytes(uint8_t reg, uint32_t* value);
};

}  // namespace Driver

#endif  // SRC_DRIVERS_INA228_INA228_HPP_
