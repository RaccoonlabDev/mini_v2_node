/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_I2C_I2C_HPP_
#define SRC_PERIPHERY_I2C_I2C_HPP_

#include <cstdint>

namespace HAL {

class I2C {
public:
    /**
     * @brief Reinitialize I2C
     * @return 0 on success, otherwise negative error code
     */
    static int8_t init();

    /**
     * @return 0 on success, otherwise negative error code
     * @param  address Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     */
    static int8_t is_device_ready(uint16_t address, uint8_t trials = 100);

    /**
     * @return Number of transmited bytes on success, otherwise negative error code
     * @param  id Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     */
    static int8_t transmit(uint16_t id, uint8_t tx[], uint8_t len);

    /**
     * @return Number of received bytes on success, otherwise negative error code
     * @param  id Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     */
    static int8_t receive(uint16_t id, uint8_t *rx, uint8_t len);

    /**
     * @return Register value [0, 255] on success, otherwise negative error code
     * @param  device_id Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     */
    static int32_t read_register_1_byte(uint16_t device_id, uint8_t reg_address);

    /**
     * @return Register value [0, 65335] on success, otherwise negative error code
     * @param  device_id Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     */
    static int32_t read_register_2_bytes(uint16_t device_id, uint8_t reg_address);
};

}  // namespace HAL

#endif  // SRC_PERIPHERY_I2C_I2C_HPP_
