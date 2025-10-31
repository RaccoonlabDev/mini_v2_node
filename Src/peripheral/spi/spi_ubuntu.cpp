/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/spi/spi.hpp"
#include <cstring>
#include "main.h"


namespace HAL {

int8_t SPI::read_registers(std::byte& reg_address, std::byte* reg_values, uint8_t size) {
    (void)reg_address;
    memset(reg_values, 0x00, size);
    return 0;
}

int8_t SPI::read_register(std::byte& reg_address, std::byte* reg_value) {
    (void)reg_address;
    *reg_value = std::byte{0x00};
    return 0;
}

int8_t SPI::transaction(std::byte* tx, std::byte* rx, uint8_t size) {
    (void)tx;
    memset(rx, 0x00, size);
    return 0;
}
#pragma GCC diagnostic push  // Save current warning state
#pragma GCC diagnostic ignored "-Wunused-parameter"  // Disable specific warnings
int8_t SPI::write_register(std::byte& reg_address, std::byte& reg_value) {
    return 0;
}
#pragma GCC diagnostic pop
}  // namespace HAL
