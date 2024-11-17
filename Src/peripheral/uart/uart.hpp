/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_UART_UART_HPP_
#define SRC_PERIPHERAL_UART_UART_HPP_

#include <cstdint>
#include <cstddef>

namespace HAL {

class UART {
public:
    enum class Instance {
        FIRST,
        SECOND,
        AMOUNT,
    };

    /**
     * @brief Allocate resources, but not initialize yet
     */
    explicit UART(UART::Instance instance_) : instance(instance_) {}

    /**
     * @brief After updating the baudrate, you typically want to reinialize the driver 
     */
    void set_baudrate(uint32_t rate);

    /**
     * @brief RX DMA mode
     * @return 0 on success, -1 on failure
     */
    int8_t init_rx_dma(uint8_t buffer[], uint16_t size);

    /**
     * @return a value in range [0, RX_DMA_BUFFER_SIZE - 1]
     */
    size_t get_last_received_index();

private:
    Instance instance;
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_UART_UART_HPP_
