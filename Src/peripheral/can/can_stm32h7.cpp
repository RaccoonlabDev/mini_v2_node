/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "libdcnode/can_driver.h"

#include <string.h>

#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

namespace {

struct CanDriver {
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t rx_buf[8];
    uint32_t err_counter;
    uint32_t tx_counter;
    uint32_t rx_counter;
};

CanDriver driver{};
bool started = false;

void initializeTxHeader() {
    driver.tx_header = {};
    driver.tx_header.IdType = FDCAN_EXTENDED_ID;
    driver.tx_header.TxFrameType = FDCAN_DATA_FRAME;
    driver.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    driver.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    driver.tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    driver.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
}

}  // namespace

extern "C" void canDriverSetInterfaceName(const char* interface_name) {
    (void)interface_name;
}

extern "C" int16_t canDriverInit(uint32_t can_speed, uint8_t can_driver_idx) {
    (void)can_speed;
    if (can_driver_idx != CAN_DRIVER_FIRST || started) {
        return -1;
    }
    initializeTxHeader();
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_REJECT_REMOTE,
                                     FDCAN_REJECT_REMOTE) != HAL_OK ||
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
    FDCAN_RxHeaderTypeDef header{};
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &header, driver.rx_buf) != HAL_OK) {
        return 0;
    }
    if (header.IdType != FDCAN_EXTENDED_ID || header.RxFrameType != FDCAN_DATA_FRAME ||
        header.FDFormat != FDCAN_CLASSIC_CAN) {
        driver.err_counter++;
        return 0;
    }
    const uint8_t data_len = static_cast<uint8_t>(header.DataLength);
    if (data_len > sizeof(rx_frame->data)) {
        driver.err_counter++;
        return 0;
    }
    rx_frame->id = (header.Identifier & CANARD_CAN_EXT_ID_MASK) | CANARD_CAN_FRAME_EFF;
    rx_frame->data_len = data_len;
    rx_frame->iface_id = 0U;
    memcpy(rx_frame->data, driver.rx_buf, data_len);
    driver.rx_counter++;
    return 1;
}

extern "C" int16_t canDriverTransmit(const CanardCANFrame* const tx_frame,
                                       uint8_t can_driver_idx) {
    if (tx_frame == nullptr || can_driver_idx != CAN_DRIVER_FIRST || !started ||
        tx_frame->data_len > sizeof(tx_frame->data)) {
        return 0;
    }
    driver.tx_header.Identifier = tx_frame->id & CANARD_CAN_EXT_ID_MASK;
    driver.tx_header.DataLength = tx_frame->data_len;
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
    FDCAN_ProtocolStatusTypeDef protocol{};
    FDCAN_ErrorCountersTypeDef counters{};
    (void)HAL_FDCAN_GetProtocolStatus(&hfdcan1, &protocol);
    (void)HAL_FDCAN_GetErrorCounters(&hfdcan1, &counters);
    return driver.err_counter + protocol.BusOff + protocol.Warning + protocol.ErrorPassive +
           counters.TxErrorCnt + counters.RxErrorCnt;
}

extern "C" uint64_t canDriverGetRxOverflowCount() {
    return 0U;
}
