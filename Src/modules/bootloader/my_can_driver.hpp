/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef BOARDS_BOOTLOADER_MY_CAN_DRIVER_HPP_
#define BOARDS_BOOTLOADER_MY_CAN_DRIVER_HPP_

#include "cyphalNode/cyphal.hpp"
#include "o1heap.h"
#include "kocherga.hpp"
#include "kocherga_can.hpp"

std::chrono::microseconds GET_TIME_SINCE_BOOT();

class MyCANDriver final : public kocherga::can::ICANDriver {
public:
    MyCANDriver();

    void init();

    auto pop(PayloadBuffer& payload_buffer)
        -> std::optional<std::pair<std::uint32_t, std::uint8_t>> override;

    auto configure(const Bitrate&                                  bitrate,
                   const bool                                      silent,
                   const kocherga::can::CANAcceptanceFilterConfig& filter
    ) -> std::optional<Mode> override;

    auto push(const bool          force_classic_can,
              const std::uint32_t extended_can_id,
              const std::uint8_t  payload_size,
              const void* const   payload) -> bool override;


    void pollTxQueue(const std::chrono::microseconds now);

private:
    typedef void* (*MemoryAllocate)(std::size_t);
    typedef void (*MemoryDeallocate)(void*);
    kocherga::can::TxQueue<MemoryAllocate, MemoryDeallocate> tx_queue_;
    bool is_configured{false};
};

#endif  // BOARDS_BOOTLOADER_MY_CAN_DRIVER_HPP_
