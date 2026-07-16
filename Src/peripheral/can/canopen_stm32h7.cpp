#include "peripheral/can/canopen_stm32h7.hpp"

#include <string.h>

#include "main.h"

extern "C" FDCAN_HandleTypeDef hfdcan2;

namespace {

struct Context {
    uint32_t errors;
    bool started;
};

Context context{};

HAL_StatusTypeDef sendFrame(const FDCAN_TxHeaderTypeDef& header, const uint8_t* data) {
    if (hfdcan2.Init.TxFifoQueueElmtsNbr > 0U) {
        return HAL_FDCAN_AddMessageToTxFifoQ(
            &hfdcan2, &header, const_cast<uint8_t*>(data));
    }

    constexpr uint32_t MAX_TX_BUFFERS = 32U;
    const uint32_t buffer_count = hfdcan2.Init.TxBuffersNbr < MAX_TX_BUFFERS
                                      ? hfdcan2.Init.TxBuffersNbr
                                      : MAX_TX_BUFFERS;
    for (uint32_t idx = 0U; idx < buffer_count; idx++) {
        const uint32_t buffer = 1UL << idx;
        if (HAL_FDCAN_IsTxBufferMessagePending(&hfdcan2, buffer) == 0U &&
            HAL_FDCAN_AddMessageToTxBuffer(&hfdcan2, &header, data, buffer) == HAL_OK) {
            return HAL_FDCAN_EnableTxBufferRequest(&hfdcan2, buffer);
        }
    }
    return HAL_ERROR;
}

int16_t init(void* raw_context, uint32_t bitrate) {
    (void)bitrate;
    auto* ctx = static_cast<Context*>(raw_context);
    if (ctx == nullptr || ctx->started) {
        return -1;
    }
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT_REMOTE,
                                     FDCAN_REJECT_REMOTE) != HAL_OK ||
        HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        ctx->errors++;
        return -1;
    }
    ctx->started = true;
    return 0;
}

int16_t send(void* raw_context, const libcanopen::Frame& frame) {
    auto* ctx = static_cast<Context*>(raw_context);
    if (ctx == nullptr || !ctx->started || frame.id > 0x7FFU || frame.data_len > 8U ||
        frame.type != libcanopen::FrameType::DATA) {
        return -1;
    }

    FDCAN_TxHeaderTypeDef header{};
    header.Identifier = frame.id;
    header.IdType = FDCAN_STANDARD_ID;
    header.TxFrameType = FDCAN_DATA_FRAME;
    header.DataLength = frame.data_len;
    header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    header.BitRateSwitch = FDCAN_BRS_OFF;
    header.FDFormat = FDCAN_CLASSIC_CAN;
    header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    if (sendFrame(header, frame.data) != HAL_OK) {
        ctx->errors++;
        return -1;
    }
    return 1;
}

int16_t receive(void* raw_context, libcanopen::Frame& frame) {
    auto* ctx = static_cast<Context*>(raw_context);
    if (ctx == nullptr || !ctx->started) {
        return -1;
    }

    FDCAN_RxHeaderTypeDef header{};
    uint8_t data[8]{};
    if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &header, data) != HAL_OK) {
        return 0;
    }
    const uint8_t data_len = static_cast<uint8_t>(header.DataLength);
    if (header.IdType != FDCAN_STANDARD_ID || header.RxFrameType != FDCAN_DATA_FRAME ||
        header.FDFormat != FDCAN_CLASSIC_CAN || header.Identifier > 0x7FFU ||
        data_len > sizeof(frame.data)) {
        ctx->errors++;
        return 0;
    }

    frame.id = static_cast<uint16_t>(header.Identifier);
    frame.data_len = data_len;
    frame.type = libcanopen::FrameType::DATA;
    frame.timestamp_us = static_cast<uint64_t>(HAL_GetTick()) * 1000ULL;
    memcpy(frame.data, data, data_len);
    return 1;
}

uint64_t getTimeUs(void* raw_context) {
    (void)raw_context;
    return static_cast<uint64_t>(HAL_GetTick()) * 1000ULL;
}

}  // namespace

libcanopen::TransportApi canopenGetTransportApi() {
    return {
        .context = &context,
        .init = init,
        .send = send,
        .receive = receive,
        .get_time_us = getTimeUs,
    };
}
