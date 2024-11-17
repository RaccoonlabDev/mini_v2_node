/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/uart/uart.hpp"
#include "main.h"

namespace HAL {

void UART::set_baudrate(uint32_t rate) {
    (void)rate;
}

int8_t UART::init_rx_dma(uint8_t buffer[], uint16_t size) {
    (void)buffer;
    (void)size;
    return 0;
}

size_t UART:: get_last_received_index() {
    return 0;
}

}  // namespace HAL
