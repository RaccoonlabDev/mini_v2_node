/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/i2c/i2c.hpp"
#include "i2c.h"

namespace HAL {

static const constexpr uint32_t I2C_TIMEOUT = 30;

int8_t I2C::transmit(uint16_t id, uint8_t tx[], uint8_t len) {
    if (HAL_I2C_Master_Transmit(&hi2c1, id, tx, len, I2C_TIMEOUT) != HAL_OK) {
        return -1;
    }

    return len - hi2c1.XferSize;
}

int8_t I2C::receive(uint16_t id, uint8_t *rx, uint8_t len) {
    if (HAL_I2C_Master_Receive(&hi2c1, id, rx, len, I2C_TIMEOUT) != HAL_OK) {
        return -1;
    }

    return len - hi2c1.XferSize;
}

}  // namespace HAL
