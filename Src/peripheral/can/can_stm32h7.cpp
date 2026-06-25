/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "peripheral/can/can.hpp"

#include <string.h>

#include "main.h"

#ifndef NUM_OF_CAN_BUSES
    #define NUM_OF_CAN_BUSES 1
#endif

#ifndef DRONECAN_FDCAN_PRIMARY
    #define DRONECAN_FDCAN_PRIMARY 1
#endif

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

namespace HAL {

namespace {

struct CanDriver {
    FDCAN_HandleTypeDef* handler;
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t rx_buf[8];
    bool initialized;
    uint32_t err_counter;
    uint32_t tx_counter;
    uint32_t rx_counter;
};

CanDriver can_drivers[NUM_OF_CAN_BUSES] = {
#if DRONECAN_FDCAN_PRIMARY == 2
    {.handler = &hfdcan2, .tx_header = {}, .rx_buf = {}, .initialized = false,
        .err_counter = 0, .tx_counter = 0, .rx_counter = 0},
#if NUM_OF_CAN_BUSES >= 2
    {.handler = &hfdcan1, .tx_header = {}, .rx_buf = {}, .initialized = false,
        .err_counter = 0, .tx_counter = 0, .rx_counter = 0}
#endif
#else
    {.handler = &hfdcan1, .tx_header = {}, .rx_buf = {}, .initialized = false,
        .err_counter = 0, .tx_counter = 0, .rx_counter = 0},
#if NUM_OF_CAN_BUSES >= 2
    {.handler = &hfdcan2, .tx_header = {}, .rx_buf = {}, .initialized = false,
        .err_counter = 0, .tx_counter = 0, .rx_counter = 0}
#endif
#endif
};

uint8_t dlcToLength(const uint32_t dlc) {
    switch (dlc) {
        case FDCAN_DLC_BYTES_0:
            return 0U;
        case FDCAN_DLC_BYTES_1:
            return 1U;
        case FDCAN_DLC_BYTES_2:
            return 2U;
        case FDCAN_DLC_BYTES_3:
            return 3U;
        case FDCAN_DLC_BYTES_4:
            return 4U;
        case FDCAN_DLC_BYTES_5:
            return 5U;
        case FDCAN_DLC_BYTES_6:
            return 6U;
        case FDCAN_DLC_BYTES_7:
            return 7U;
        case FDCAN_DLC_BYTES_8:
        default:
            return 8U;
    }
}

uint32_t lengthToDlc(const uint8_t length) {
    switch (length) {
        case 0U:
            return FDCAN_DLC_BYTES_0;
        case 1U:
            return FDCAN_DLC_BYTES_1;
        case 2U:
            return FDCAN_DLC_BYTES_2;
        case 3U:
            return FDCAN_DLC_BYTES_3;
        case 4U:
            return FDCAN_DLC_BYTES_4;
        case 5U:
            return FDCAN_DLC_BYTES_5;
        case 6U:
            return FDCAN_DLC_BYTES_6;
        case 7U:
            return FDCAN_DLC_BYTES_7;
        case 8U:
        default:
            return FDCAN_DLC_BYTES_8;
    }
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

bool initPhysicalCan(const size_t physical_idx) {
    if (physical_idx >= NUM_OF_CAN_BUSES || can_drivers[physical_idx].handler == nullptr) {
        return false;
    }
    if (can_drivers[physical_idx].initialized) {
        return true;
    }

    initTxHeader(can_drivers[physical_idx].tx_header);

    HAL_StatusTypeDef status = HAL_FDCAN_ConfigGlobalFilter(
        can_drivers[physical_idx].handler,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_REJECT_REMOTE,
        FDCAN_REJECT_REMOTE);
    if (status != HAL_OK) {
        can_drivers[physical_idx].err_counter++;
        return false;
    }

    status = HAL_FDCAN_Start(can_drivers[physical_idx].handler);
    if (status != HAL_OK) {
        can_drivers[physical_idx].err_counter++;
        return false;
    }

    can_drivers[physical_idx].initialized = true;
    return true;
}

size_t instanceToFirstPhysical(const Can::Instance instance) {
    if (instance == Can::Instance::INSTANCE_2 && NUM_OF_CAN_BUSES >= 2) {
        return 1U;
    }
    return 0U;
}

size_t instanceToPhysicalCount(const Can::Instance instance) {
    if (instance == Can::Instance::INSTANCE_1) {
        return NUM_OF_CAN_BUSES;
    }
    return (NUM_OF_CAN_BUSES >= 2) ? 1U : 0U;
}

}  // namespace

Can::Can(Instance instance) : _instance(instance) {}

Can::ErrorCode Can::init(const InitConfig& config) {
    (void)config.baud_rate;

    const size_t first_physical_idx = instanceToFirstPhysical(_instance);
    const size_t physical_count = instanceToPhysicalCount(_instance);
    if (physical_count == 0U || first_physical_idx >= NUM_OF_CAN_BUSES) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    for (size_t offset = 0U; offset < physical_count; offset++) {
        if (!initPhysicalCan(first_physical_idx + offset)) {
            _stats.errors++;
            return ErrorCode::INITIALIZATION_FAILED;
        }
    }

    _is_initialized = true;
    return ErrorCode::SUCCESS;
}

std::variant<Can::ClassicFrame, Can::ErrorCode> Can::receive() {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    const size_t first_physical_idx = instanceToFirstPhysical(_instance);
    const size_t physical_count = instanceToPhysicalCount(_instance);
    static size_t next_physical_offset = 0U;

    for (size_t attempt = 0U; attempt < physical_count; attempt++) {
        const size_t physical_idx =
            first_physical_idx + ((next_physical_offset + attempt) % physical_count);
        CanDriver& driver = can_drivers[physical_idx];
        if (!driver.initialized) {
            continue;
        }

        FDCAN_RxHeaderTypeDef rx_header{};
        const HAL_StatusTypeDef status = HAL_FDCAN_GetRxMessage(driver.handler,
                                                                 FDCAN_RX_FIFO0,
                                                                 &rx_header,
                                                                 driver.rx_buf);
        if (status != HAL_OK) {
            continue;
        }

        ClassicFrame frame{};
        frame.id = rx_header.Identifier;
        frame.data_len = dlcToLength(rx_header.DataLength);
        frame.timestamp_ns = static_cast<uint64_t>(HAL_GetTick()) * 1000000ULL;
        memcpy(frame.data, driver.rx_buf, frame.data_len);

        driver.rx_counter++;
        _stats.rx_frames++;
        next_physical_offset = (physical_idx - first_physical_idx + 1U) % physical_count;
        return frame;
    }

    return ErrorCode::TIMEOUT_OR_FIFO_EMPTY;
}

Can::ErrorCode Can::send(const ClassicFrame& frame) {
    if (!_is_initialized) {
        _stats.errors++;
        return ErrorCode::INITIALIZATION_FAILED;
    }

    const size_t first_physical_idx = instanceToFirstPhysical(_instance);
    const size_t physical_count = instanceToPhysicalCount(_instance);
    bool sent = false;

    for (size_t offset = 0U; offset < physical_count; offset++) {
        CanDriver& driver = can_drivers[first_physical_idx + offset];
        if (!driver.initialized) {
            continue;
        }

        driver.tx_header.Identifier = frame.id;
        driver.tx_header.DataLength = lengthToDlc(frame.data_len);

        const HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(driver.handler,
                                                                &driver.tx_header,
                                                                (uint8_t*)(frame.data));
        if (status == HAL_OK) {
            driver.tx_counter++;
            sent = true;
        } else {
            driver.err_counter++;
        }
    }

    if (!sent) {
        _stats.errors++;
        return ErrorCode::TRANSMISSION_ERROR;
    }

    _stats.tx_frames++;
    return ErrorCode::SUCCESS;
}

}  // namespace HAL
