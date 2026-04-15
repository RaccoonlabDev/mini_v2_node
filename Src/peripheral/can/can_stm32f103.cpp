/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/can/can.hpp"

#include <string.h>
#include <limits>

extern "C" {
#include "canard_stm32.h"
}

#include "can.h"
#include "main.h"
#include "peripheral/can/ring_buffer.hpp"

namespace HAL {

namespace {

static constexpr size_t RX_RING_CAPACITY = 128U;
static RingBuffer<Can::ClassicFrame, RX_RING_CAPACITY> rx_ring_buffer;
static volatile uint32_t software_rx_overflow_count = 0U;
static volatile uint32_t local_error_count = 0U;

// Avoid deprecated ++ on volatile objects.
void incrementVolatileCounter(volatile uint32_t& counter) {
    counter = counter + 1U;
}

uint32_t saturateToU32(const uint64_t value) {
    const uint64_t max_u32 = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
    return static_cast<uint32_t>(value > max_u32 ? max_u32 : value);
}

void refreshDriverStats(HAL::Can::CanBusStats& stats) {
    const auto driver_stats = canardSTM32GetStats();
    stats.rx_overflow =
        saturateToU32(driver_stats.rx_overflow_count +
                      static_cast<uint64_t>(software_rx_overflow_count));
    stats.errors =
        saturateToU32(driver_stats.error_count + static_cast<uint64_t>(local_error_count));
}

bool waitForAllTxMailboxesFree(const uint32_t timeout_ms) {
    const uint32_t started_at = HAL_GetTick();
    while (true) {
        if ((hcan.Instance->TSR & (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2)) ==
            (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2)) {
            return true;
        }
        if ((HAL_GetTick() - started_at) >= timeout_ms) {
            return false;
        }
    }
}

void pushFrameToRxRing(const CanardCANFrame& rx_frame) {
    Can::ClassicFrame frame{};
    frame.id = static_cast<uint32_t>(rx_frame.id & CANARD_CAN_EXT_ID_MASK);
    frame.data_len = static_cast<uint8_t>(rx_frame.data_len);
    if (frame.data_len > sizeof(frame.data)) {
        frame.data_len = sizeof(frame.data);
    }
    memcpy(frame.data, rx_frame.data, frame.data_len);
    frame.timestamp_ns = static_cast<uint64_t>(HAL_GetTick()) * 1000000ULL;

    if (rx_ring_buffer.getSize() >= RX_RING_CAPACITY) {
        incrementVolatileCounter(software_rx_overflow_count);
    }
    rx_ring_buffer.push(frame);
}

void drainHardwareRxFifos() {
    while (true) {
        CanardCANFrame rx_frame{};
        const int16_t result = canardSTM32Receive(&rx_frame);
        if (result <= 0) {
            if (result < 0) {
                incrementVolatileCounter(local_error_count);
            }
            return;
        }
        pushFrameToRxRing(rx_frame);
    }
}

}  // namespace

Can::Can(Instance instance) : _instance(instance) {}

Can::ErrorCode Can::init(const InitConfig& config) {
    if (_instance != Instance::INSTANCE_1) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    __disable_irq();
    rx_ring_buffer = RingBuffer<ClassicFrame, RX_RING_CAPACITY>{};
    software_rx_overflow_count = 0U;
    local_error_count = 0U;
    __enable_irq();

    CanardSTM32CANTimings timings{};
    const int16_t timings_result =
        canardSTM32ComputeCANTimings(HAL_RCC_GetPCLK1Freq(), config.baud_rate, &timings);
    if (timings_result < 0) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (canardSTM32Init(&timings, CanardSTM32IfaceModeNormal) < 0) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    const CanardSTM32AcceptanceFilterConfiguration filter{.id = 0U, .mask = 0U};
    if (canardSTM32ConfigureAcceptanceFilters(&filter, 1U) < 0) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    hcan.Instance->IER |= CAN_IER_FMPIE0 | CAN_IER_FOVIE0 | CAN_IER_FMPIE1 | CAN_IER_FOVIE1 |
                          CAN_IER_ERRIE | CAN_IER_EPVIE | CAN_IER_EWGIE | CAN_IER_BOFIE |
                          CAN_IER_LECIE;
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0U, 0U);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0U, 0U);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0U, 0U);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

    _is_initialized = true;
    refreshDriverStats(_stats);
    return ErrorCode::SUCCESS;
}

std::variant<Can::ClassicFrame, Can::ErrorCode> Can::receive() {
    if (!_is_initialized) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    __disable_irq();
    const bool has_frame = rx_ring_buffer.getSize() > 0U;
    if (!has_frame) {
        __enable_irq();
        refreshDriverStats(_stats);
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }
    ClassicFrame frame = rx_ring_buffer.pop();
    __enable_irq();

    _stats.rx_frames++;
    refreshDriverStats(_stats);
    return frame;
}

Can::ErrorCode Can::send(const ClassicFrame& frame) {
    if (!_is_initialized) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::INITIALIZATION_FAILED;
    }

    CanardCANFrame tx_frame{};
    tx_frame.id = (frame.id & CANARD_CAN_EXT_ID_MASK) | CANARD_CAN_FRAME_EFF;
    tx_frame.data_len = frame.data_len;
    if (tx_frame.data_len > sizeof(tx_frame.data)) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::TRANSMISSION_ERROR;
    }
    memcpy(tx_frame.data, frame.data, tx_frame.data_len);

    const int16_t result = canardSTM32Transmit(&tx_frame);
    if (result <= 0) {
        if (result < 0) {
            incrementVolatileCounter(local_error_count);
            refreshDriverStats(_stats);
            return ErrorCode::TRANSMISSION_ERROR;
        }
        refreshDriverStats(_stats);
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }

    if (!waitForAllTxMailboxesFree(10U)) {
        incrementVolatileCounter(local_error_count);
        refreshDriverStats(_stats);
        return ErrorCode::TRANSMISSION_ERROR;
    }

    _stats.tx_frames++;
    refreshDriverStats(_stats);
    return ErrorCode::SUCCESS;
}

}  // namespace HAL

extern "C" void can_stm32f103_on_rx_irq(void) {
    HAL::drainHardwareRxFifos();
}

extern "C" void can_stm32f103_on_sce_irq(void) {
    HAL::drainHardwareRxFifos();
}
