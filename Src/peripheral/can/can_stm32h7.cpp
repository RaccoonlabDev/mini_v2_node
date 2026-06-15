/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "peripheral/can/can.hpp"

#include <string.h>

#include "main.h"
#include "peripheral/can/ring_buffer.hpp"

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

namespace HAL {

namespace {

static constexpr size_t CAN_INSTANCE_COUNT = 2U;
static constexpr size_t RX_RING_CAPACITY = 128U;

struct CanDriver {
    FDCAN_HandleTypeDef* handler;
    FDCAN_TxHeaderTypeDef tx_header;
};

static CanDriver can_drivers[CAN_INSTANCE_COUNT]{};
static RingBuffer<Can::ClassicFrame, RX_RING_CAPACITY> rx_ring_buffers[CAN_INSTANCE_COUNT];
static volatile uint32_t software_rx_overflow_count[CAN_INSTANCE_COUNT]{};

size_t instanceToIndex(const Can::Instance instance) {
    return static_cast<size_t>(instance);
}

FDCAN_HandleTypeDef* instanceToHandle(const Can::Instance instance) {
    switch (instance) {
        case Can::Instance::INSTANCE_1:
            return &hfdcan1;
        case Can::Instance::INSTANCE_2:
            return &hfdcan2;
        default:
            return nullptr;
    }
}

uint8_t dlcToLength(const uint32_t dlc) {
    const uint8_t length = static_cast<uint8_t>(dlc >> 16U);
    return length > 8U ? 8U : length;
}

uint32_t lengthToDlc(const uint8_t length) {
    const uint8_t sanitized_length = length > 8U ? 8U : length;
    return static_cast<uint32_t>(sanitized_length) << 16U;
}

void initTxHeader(FDCAN_TxHeaderTypeDef& tx_header) {
    tx_header.IdType = FDCAN_EXTENDED_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;
}

bool configureAcceptAllFilter(FDCAN_HandleTypeDef* const handler) {
    FDCAN_FilterTypeDef filter_config{};
    filter_config.IdType = FDCAN_EXTENDED_ID;
    filter_config.FilterIndex = 0;
    filter_config.FilterType = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_DISABLE;
    return HAL_FDCAN_ConfigFilter(handler, &filter_config) == HAL_OK;
}

}  // namespace

void canOnRxFifo0Callback(FDCAN_HandleTypeDef* const hfdcan) {
    if (hfdcan == nullptr) {
        return;
    }

    size_t can_idx = CAN_INSTANCE_COUNT;
    for (size_t idx = 0U; idx < CAN_INSTANCE_COUNT; idx++) {
        if (can_drivers[idx].handler == hfdcan) {
            can_idx = idx;
            break;
        }
    }
    if (can_idx >= CAN_INSTANCE_COUNT) {
        return;
    }

    FDCAN_RxHeaderTypeDef rx_header{};
    uint8_t rx_buf[8]{};
    const uint32_t fifo_size = HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0);
    for (uint32_t idx = 0U; idx < fifo_size; idx++) {
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_buf) != HAL_OK) {
            continue;
        }

        Can::ClassicFrame frame{};
        frame.id = rx_header.Identifier;
        frame.data_len = dlcToLength(rx_header.DataLength);
        frame.timestamp_ns = static_cast<uint64_t>(HAL_GetTick()) * 1000000ULL;
        memcpy(frame.data, rx_buf, frame.data_len);

        if (rx_ring_buffers[can_idx].getSize() >= RX_RING_CAPACITY) {
            software_rx_overflow_count[can_idx] = software_rx_overflow_count[can_idx] + 1U;
        }
        rx_ring_buffers[can_idx].push(frame);
    }
}

Can::Can(Instance instance) : _instance(instance) {}

Can::ErrorCode Can::init(const InitConfig& config) {
    (void)config.baud_rate;

    const size_t can_idx = instanceToIndex(_instance);
    if (can_idx >= CAN_INSTANCE_COUNT) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    FDCAN_HandleTypeDef* const handler = instanceToHandle(_instance);
    if (handler == nullptr) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    __disable_irq();
    rx_ring_buffers[can_idx] = RingBuffer<ClassicFrame, RX_RING_CAPACITY>{};
    software_rx_overflow_count[can_idx] = 0U;
    __enable_irq();

    can_drivers[can_idx].handler = handler;
    initTxHeader(can_drivers[can_idx].tx_header);

    if (!configureAcceptAllFilter(handler)) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (HAL_FDCAN_Start(handler) != HAL_OK) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (HAL_FDCAN_ActivateNotification(handler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0U) != HAL_OK) {
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

    const size_t can_idx = instanceToIndex(_instance);
    if (can_idx >= CAN_INSTANCE_COUNT) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    __disable_irq();
    const bool has_frame = rx_ring_buffers[can_idx].getSize() > 0U;
    if (!has_frame) {
        __enable_irq();
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }
    ClassicFrame frame = rx_ring_buffers[can_idx].pop();
    __enable_irq();

    _stats.rx_frames++;
    _stats.rx_overflow = software_rx_overflow_count[can_idx];
    return frame;
}

Can::ErrorCode Can::send(const ClassicFrame& frame) {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    const size_t can_idx = instanceToIndex(_instance);
    if (can_idx >= CAN_INSTANCE_COUNT || can_drivers[can_idx].handler == nullptr) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    can_drivers[can_idx].tx_header.Identifier = frame.id;
    can_drivers[can_idx].tx_header.DataLength = lengthToDlc(frame.data_len);

    HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(can_drivers[can_idx].handler,
                                                              &can_drivers[can_idx].tx_header,
                                                              const_cast<uint8_t*>(frame.data));
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
