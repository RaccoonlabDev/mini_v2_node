/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_I2C_I2C_HPP_
#define SRC_PERIPHERY_I2C_I2C_HPP_

#include <cstdint>

#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT 30
#endif

class I2CPeriphery {
  public:
    /**
     * @return Number of transmited bytes on success, otherwise < 0
     */
    static int8_t transmit(uint16_t id, uint8_t tx[], uint8_t len);

    /**
     * @return Number of received bytes on success, otherwise < 0
     */
    static int8_t receive(uint16_t id, uint8_t *rx, uint8_t len);
};

#endif // SRC_PERIPHERY_I2C_I2C_HPP_
