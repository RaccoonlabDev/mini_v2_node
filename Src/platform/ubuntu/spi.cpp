/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/spi/spi.hpp"
#include <cstring>
#include "main.h"

namespace HAL {

int8_t SPI::read_registers(uint8_t reg_address, uint8_t* reg_values, uint8_t size) {
    (void)reg_address;
    (void)reg_values;
    (void)size;
    return -1;
}

int8_t SPI::read_register(uint8_t reg_address, uint8_t* reg_value) {
    (void)reg_address;
    (void)reg_value;
    return -1;
}

int8_t SPI::transaction(uint8_t* tx, uint8_t* rx, uint8_t size) {
    (void)tx;
    (void)rx;
    (void)size;
    return -1;
}

}  // namespace HAL
