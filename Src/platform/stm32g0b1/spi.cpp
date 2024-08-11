/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/spi/spi.hpp"
#include <cstring>
#include "main.h"

static constexpr uint32_t MEASUREMENT_DELAY = 10;
static constexpr uint8_t SPI_READ = 0x80;

extern SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef* hspi = &hspi2;

namespace HAL {

static void spi_set_nss(bool nss_state) {
#ifdef SPI2_NSS_GPIO_Port
    auto state = nss_state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, state);
#endif
}

int8_t SPI::read_registers(uint8_t reg_address, uint8_t* reg_values, uint8_t size) {
    if (reg_values == nullptr) {
        return -1;
    }

    uint8_t tx_byte = reg_address | SPI_READ;
    return HAL::SPI::transaction(&tx_byte, &reg_values[-1], size + 1);
}

int8_t SPI::read_register(uint8_t reg_address, uint8_t* reg_value) {
    if (reg_value == nullptr) {
        return -1;
    }

    uint8_t tx_byte = reg_address | SPI_READ;
    return HAL::SPI::transaction(&tx_byte, &reg_value[-1], 2);
}

int8_t SPI::transaction(uint8_t* tx, uint8_t* rx, uint8_t size) {
    if (tx == nullptr || rx == nullptr) {
        return -1;
    }

    spi_set_nss(false);
    memset(rx, 0x00, size);
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hspi, tx, rx, size, MEASUREMENT_DELAY);
    spi_set_nss(true);

    return (status == HAL_OK) ? 0 : -status;
}

}  // namespace HAL
