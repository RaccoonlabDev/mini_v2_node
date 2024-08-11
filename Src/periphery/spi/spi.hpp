/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_SPI_SPI_HPP_
#define SRC_PERIPHERY_SPI_SPI_HPP_

#include <cstdint>

namespace HAL {

class SPI {
public:
    /**
     * @brief Read a single register
     * @param[in] reg_address - a value withing [0, 127]
     * @param[out] reg_value - a register value will be written here on successfull transacation
     * @return 0 on success, negative error otherwise
     * @note The function automatically adds SPI_READ flag
     */
    static int8_t read_register(uint8_t reg_address, uint8_t* reg_value);

    /**
     * @brief Read multiple registers
     * @param[in] reg_address - a value withing [0, 127]
     * @param[out] reg_values - a register value will be written here on successfull transacation
     * @param[in] size - how many registers to read
     * @return 0 on success, negative error otherwise
     * @note The function automatically adds SPI_READ flag
     */
    static int8_t read_registers(uint8_t reg_address, uint8_t* reg_values, uint8_t size);

    /**
     * @brief This function automayically controls the NSS pin
     * @return 0 on success, negative error otherwise
     */
    static int8_t transaction(uint8_t* tx, uint8_t* rx, uint8_t size);
};

}  // namespace HAL

#endif  // SRC_PERIPHERY_SPI_SPI_HPP_
