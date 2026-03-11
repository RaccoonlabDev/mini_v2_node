/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/can/can.hpp"

#include <string.h>

#include "main.h"
#include "peripheral/can/ring_buffer.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

namespace HAL {

typedef struct {
    FDCAN_HandleTypeDef* handler;
    FDCAN_TxHeaderTypeDef tx_header;
} CanDriver;

static CanDriver can_driver{};
static RingBuffer<Can::ClassicFrame, 128U> rx_ring_buffer;

void canOnRxFifo0Callback(FDCAN_HandleTypeDef* const hfdcan) {
    if (hfdcan == nullptr || can_driver.handler == nullptr || hfdcan != can_driver.handler) {
        return;
    }

    FDCAN_RxHeaderTypeDef rx_header{};
    uint8_t rx_buf[8]{};
    const uint8_t fifo_size = HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0);
    for (uint8_t idx = 0U; idx < fifo_size; idx++) {
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_buf) != HAL_OK) {
            continue;
        }

        Can::ClassicFrame frame{};
        frame.id = rx_header.Identifier;
        frame.data_len = static_cast<uint8_t>(rx_header.DataLength >> 16U);
        if (frame.data_len > 8U) {
            frame.data_len = 8U;
        }
        frame.timestamp_ns = static_cast<uint64_t>(HAL_GetTick()) * 1000000ULL;
        memcpy(frame.data, rx_buf, frame.data_len);
        rx_ring_buffer.push(frame);
    }
}

Can::Can(Instance instance) : _instance(instance) {}

Can::ErrorCode Can::init(const InitConfig& config) {
    (void)config.baud_rate;

    if (_instance != Instance::INSTANCE_1) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    can_driver.handler = &hfdcan1;
    can_driver.tx_header.IdType = FDCAN_EXTENDED_ID;
    can_driver.tx_header.TxFrameType = FDCAN_DATA_FRAME;
    can_driver.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    can_driver.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    can_driver.tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    can_driver.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    can_driver.tx_header.MessageMarker = 0;

    FDCAN_FilterTypeDef filter_config;
    filter_config.IdType = FDCAN_EXTENDED_ID;
    filter_config.FilterIndex = 0;
    filter_config.FilterType = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_DISABLE;

    if (HAL_FDCAN_ConfigFilter(can_driver.handler, &filter_config) != HAL_OK) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (HAL_FDCAN_Start(can_driver.handler) != HAL_OK) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (HAL_FDCAN_ActivateNotification(can_driver.handler,
                                       FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
                                       0U) != HAL_OK) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    _is_initialized = true;
    return ErrorCode::SUCCESS;
}

std::variant<Can::ClassicFrame, Can::ErrorCode> Can::receive() {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (rx_ring_buffer.getSize() == 0U) {
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }

    __disable_irq();
    ClassicFrame frame = rx_ring_buffer.pop();
    __enable_irq();

    _stats.rx_frames++;
    return frame;
}

Can::ErrorCode Can::send(const ClassicFrame& frame) {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    can_driver.tx_header.Identifier = frame.id;
    can_driver.tx_header.DataLength = frame.data_len << 16;

    HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(can_driver.handler,
                                                              &can_driver.tx_header,
                                                              (uint8_t*)frame.data);
    if (status != HAL_OK) {
        _stats.errors++;
        return ErrorCode::TRANSMISSION_ERROR;
    }

    _stats.tx_frames++;
    return ErrorCode::SUCCESS;
}

}  // namespace HAL

extern "C" void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan,
                                          uint32_t RxFifo0ITs) {
    (void)RxFifo0ITs;
    HAL::canOnRxFifo0Callback(hfdcan);
}
