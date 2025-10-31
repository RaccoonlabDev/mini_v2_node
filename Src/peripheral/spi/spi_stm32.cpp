/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/spi/spi.hpp"
#include <array>
#include <cstring>
#include <vector>
#include "main.h"

static constexpr uint32_t TRANSMIT_DELAY = 100;  // 100 is experimental value. Need to prove it
static constexpr std::byte SPI_READ{0x80};

extern SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef* hspi = &hspi2;

namespace HAL {

static void spi_set_nss(bool nss_state) {
#ifdef SPI2_NSS_GPIO_Port
    auto state = nss_state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, state);
#elif defined(SPI_SS_GPIO_Port)
    auto state = nss_state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, state);
#endif
}

int8_t SPI::read_registers(std::byte& reg_address, std::byte* reg_values, uint8_t size) {
    if (reg_values == nullptr || size > MAX_SPI_REGISTERS_READ) {
        return -1;
    }
    std::array<std::byte, MAX_SPI_REGISTERS_READ + 1> tx_buffer = {};
    std::array<std::byte, MAX_SPI_REGISTERS_READ + 1> rx_buffer = {};
    tx_buffer[0] = reg_address | SPI_READ;

    auto result = transaction(tx_buffer.data(), rx_buffer.data(), size + 1);
    if (result == 0) {
        // Copy actual data (skip first dummy byte)
        // analog from just read_register where we start reading actual data from 2nd byte
        std::copy(rx_buffer.begin() + 1, rx_buffer.begin() + size + 1, reg_values);
    }
    return result;
}

int8_t SPI::read_register(std::byte& reg_address, std::byte* reg_value) {
    if (reg_value == nullptr) {
        return -1;
    }
    std::array <std::byte, 2> tx_buffer = {reg_address | SPI_READ, std::byte{0}};
    std::array <std::byte, 2> rx_buffer = {std::byte{0}, std::byte{0}};
    auto result = transaction(tx_buffer.data(), rx_buffer.data(), 2);
    if (result == 0) {
        *reg_value = rx_buffer[1];
    }
    return result;
}

int8_t SPI::write_register(std::byte& reg_address, std::byte& reg_value) {
    std::array<std::byte, 2> tx_buffer = {reg_address, reg_value};
    std::array<std::byte, 2> rx_buffer = {std::byte{0}, std::byte{0}};
    return HAL::SPI::transaction(tx_buffer.data(), rx_buffer.data(), tx_buffer.size());
}

int8_t SPI::transaction(std::byte* tx, std::byte* rx, uint8_t size) {
    if (tx == nullptr || rx == nullptr) {
        return -1;
    }

    spi_set_nss(false);
    memset(rx, 0x00, size);
    auto status = HAL_SPI_TransmitReceive(hspi,
                                          reinterpret_cast<uint8_t*>(tx),
                                          reinterpret_cast<uint8_t*>(rx),
                                          size,
                                          TRANSMIT_DELAY
    );
    spi_set_nss(true);

    return (status == HAL_OK) ? 0 : -status;
}

}  // namespace HAL
