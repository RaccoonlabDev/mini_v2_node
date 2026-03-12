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

namespace HAL {

namespace {

void updateOverflowCounter(HAL::Can::CanBusStats& stats) {
    const uint64_t raw = canardSTM32GetStats().rx_overflow_count;
    const uint64_t max_u32 = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
    stats.rx_overflow = static_cast<uint32_t>(raw > max_u32 ? max_u32 : raw);
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

}  // namespace

Can::Can(Instance instance) : _instance(instance) {}

Can::ErrorCode Can::init(const InitConfig& config) {
    if (_instance != Instance::INSTANCE_1) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    CanardSTM32CANTimings timings{};
    const int16_t timings_result =
        canardSTM32ComputeCANTimings(HAL_RCC_GetPCLK1Freq(), config.baud_rate, &timings);
    if (timings_result < 0) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    if (canardSTM32Init(&timings, CanardSTM32IfaceModeNormal) < 0) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    const CanardSTM32AcceptanceFilterConfiguration filter{.id = 0U, .mask = 0U};
    if (canardSTM32ConfigureAcceptanceFilters(&filter, 1U) < 0) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    _is_initialized = true;
    updateOverflowCounter(_stats);
    return ErrorCode::SUCCESS;
}

std::variant<Can::ClassicFrame, Can::ErrorCode> Can::receive() {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }
    updateOverflowCounter(_stats);

    CanardCANFrame rx_frame{};
    const int16_t result = canardSTM32Receive(&rx_frame);
    if (result < 0) {
        _stats.errors++;
        return ErrorCode::RECEPTION_ERROR;
    }
    if (result == 0) {
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }

    ClassicFrame frame{};
    frame.id = static_cast<uint32_t>(rx_frame.id & CANARD_CAN_EXT_ID_MASK);
    frame.data_len = static_cast<uint8_t>(rx_frame.data_len);
    if (frame.data_len > sizeof(frame.data)) {
        frame.data_len = sizeof(frame.data);
    }
    memcpy(frame.data, rx_frame.data, frame.data_len);
    frame.timestamp_ns = static_cast<uint64_t>(HAL_GetTick()) * 1000000ULL;

    _stats.rx_frames++;
    return frame;
}

Can::ErrorCode Can::send(const ClassicFrame& frame) {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }
    updateOverflowCounter(_stats);

    CanardCANFrame tx_frame{};
    tx_frame.id = (frame.id & CANARD_CAN_EXT_ID_MASK) | CANARD_CAN_FRAME_EFF;
    tx_frame.data_len = frame.data_len;
    if (tx_frame.data_len > sizeof(tx_frame.data)) {
        _stats.errors++;
        return ErrorCode::TRANSMISSION_ERROR;
    }
    memcpy(tx_frame.data, frame.data, tx_frame.data_len);

    const int16_t result = canardSTM32Transmit(&tx_frame);
    if (result <= 0) {
        if (result < 0) {
            _stats.errors++;
            return ErrorCode::TRANSMISSION_ERROR;
        }
        return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
    }

    if (!waitForAllTxMailboxesFree(10U)) {
        _stats.errors++;
        return ErrorCode::TRANSMISSION_ERROR;
    }

    _stats.tx_frames++;
    return ErrorCode::SUCCESS;
}

}  // namespace HAL
