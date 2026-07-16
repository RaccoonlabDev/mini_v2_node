/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "libdcnode/can_driver.h"

#include <string.h>

#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

namespace {

struct CanDriver {
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t rx_buf[8];
    size_t err_counter;
    size_t tx_counter;
    size_t rx_counter;
};

CanDriver driver{};
bool started = false;

}  // namespace

extern "C" void canDriverSetInterfaceName(const char* interface_name) {
    (void)interface_name;
}

extern "C" int16_t canDriverInit(uint32_t can_speed, uint8_t can_driver_idx) {
    (void)can_speed;
    if (can_driver_idx != CAN_DRIVER_FIRST || started) {
        return -1;
    }

    driver.tx_header.IdType = FDCAN_EXTENDED_ID;
    driver.tx_header.TxFrameType = FDCAN_DATA_FRAME;
    driver.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    driver.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    driver.tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    driver.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    driver.tx_header.MessageMarker = 0U;

    FDCAN_FilterTypeDef filter_config{};
    filter_config.IdType = FDCAN_EXTENDED_ID;
    filter_config.FilterIndex = 0U;
    filter_config.FilterType = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_DISABLE;

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config) != HAL_OK ||
        HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        driver.err_counter++;
        return -1;
    }

    started = true;
    return 0;
}

extern "C" int16_t canDriverReceive(CanardCANFrame* const rx_frame,
                                      uint8_t can_driver_idx) {
    if (rx_frame == nullptr || can_driver_idx != CAN_DRIVER_FIRST || !started) {
        return 0;
    }

    FDCAN_RxHeaderTypeDef rx_header{};
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rx_header, driver.rx_buf) != HAL_OK) {
        return 0;
    }

    driver.rx_counter++;
    rx_frame->id = (CANARD_CAN_EXT_ID_MASK & rx_header.Identifier) | CANARD_CAN_FRAME_EFF;
    rx_frame->data_len = static_cast<uint8_t>(rx_header.DataLength >> 16U);
    rx_frame->iface_id = 0U;
    if (rx_frame->data_len > sizeof(rx_frame->data)) {
        driver.err_counter++;
        return 0;
    }
    memcpy(rx_frame->data, driver.rx_buf, rx_frame->data_len);
    return 1;
}

extern "C" int16_t canDriverTransmit(const CanardCANFrame* const tx_frame,
                                       uint8_t can_driver_idx) {
    if (tx_frame == nullptr || can_driver_idx != CAN_DRIVER_FIRST || !started ||
        tx_frame->data_len > sizeof(tx_frame->data)) {
        return 0;
    }

    driver.tx_header.Identifier = tx_frame->id & CANARD_CAN_EXT_ID_MASK;
    driver.tx_header.DataLength = static_cast<uint32_t>(tx_frame->data_len) << 16U;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1,
                                      &driver.tx_header,
                                      const_cast<uint8_t*>(tx_frame->data)) != HAL_OK) {
        driver.err_counter++;
        return 0;
    }

    driver.tx_counter++;
    return 1;
}

extern "C" uint64_t canDriverGetErrorCount() {
    return driver.err_counter;
}

extern "C" uint64_t canDriverGetRxOverflowCount() {
    return 0U;
}
