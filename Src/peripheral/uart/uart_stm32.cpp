/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/uart/uart.hpp"
#include "main.h"

extern UART_HandleTypeDef huart1;
#define UART_1_PTR &huart1

#if defined(SECOND_UART)
    extern UART_HandleTypeDef huart2;
    #define UART_2_PTR &huart2
#else
    #define UART_2_PTR NULL
#endif

typedef enum {
    NO_FLAGS = 0,
    HALF_RECEIVED_FLAG,
    FULL_RECEIVED_FLAG,
    BOTH_FLAGS,
} UartRxStatus_t;

typedef struct {
    UART_HandleTypeDef* huart_ptr;
    uint8_t* buffer;
    uint16_t size;
    UartRxStatus_t status;
    void (*rx_callback)();
    uint32_t rx_counter;
} UartRxConfig_t;

static UartRxConfig_t uart_rx[2] = {
    {
        .huart_ptr = UART_1_PTR,
        .buffer = nullptr,
        .size = 0,
        .status = NO_FLAGS,
        .rx_callback = nullptr,
        .rx_counter = 0
    },
    {
        .huart_ptr = UART_2_PTR,
        .buffer = nullptr,
        .size = 0,
        .status = NO_FLAGS,
        .rx_callback = nullptr,
        .rx_counter = 0
    },
};

namespace HAL {

void UART::set_baudrate(uint32_t rate) {
    if (instance == Instance::FIRST) {
        huart1.Init.BaudRate = rate;
        HAL_UART_Init(&huart1);
    } else if (instance == Instance::SECOND) {
#if defined(SECOND_UART)
        huart2.Init.BaudRate = rate;
        HAL_HalfDuplex_Init(&huart2);
#endif
    }
}


int8_t UART::init_rx_dma(uint8_t buffer[], uint16_t size) {
    if (instance >= Instance::AMOUNT) {
        return -1;
    }

    auto& uart_rx_config = uart_rx[static_cast<uint8_t>(instance)];

    uart_rx_config.buffer = buffer;
    uart_rx_config.size = size;
    uart_rx_config.rx_counter = 0;
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(uart_rx_config.huart_ptr, buffer, size);
    return status == HAL_OK ? 0 : -1;
}


size_t UART:: get_last_received_index() {
    if (instance >= Instance::AMOUNT) {
        return 0;
    }

    auto uart_rx_config = &uart_rx[static_cast<uint8_t>(instance)];

    if (uart_rx_config->huart_ptr == nullptr) {
        return 0;
    }

    uint16_t remaining_data_units = __HAL_DMA_GET_COUNTER(uart_rx_config->huart_ptr->hdmarx);
    if (remaining_data_units == uart_rx_config->size) {
        return uart_rx_config->size - 1;
    }
    return uart_rx_config->size - remaining_data_units - 1;
}

}  // namespace HAL
