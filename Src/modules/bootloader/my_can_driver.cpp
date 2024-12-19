/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "my_can_driver.hpp"
#include "cyphalNode/cyphal_transport_can.hpp"
#include "o1heap.h"
#include "main.h"

static cyphal::CyphalTransportCan transport_can_driver;
static uint8_t base[cyphal::HEAP_SIZE] __attribute__ ((aligned (O1HEAP_ALIGNMENT)));
static O1HeapInstance* my_allocator;

std::optional<std::pair<std::uint32_t, std::uint8_t>> CAN_POP(unsigned char* data) {
    std::optional<std::pair<std::uint32_t, std::uint8_t>> out;
    cyphal::CanardFrame can_frame;
    if (transport_can_driver.receive(&can_frame)) {
        out.emplace(std::pair<std::uint32_t, std::uint8_t>{
            can_frame.extended_can_id,
            can_frame.payload_size
        });
        memcpy(data, can_frame.payload, can_frame.payload_size);
    }
    return out;
}
bool CAN_PUSH(uint32_t extended_can_id, std::size_t size, const uint8_t arr[]) {
    cyphal::CanardTxQueueItem transfer;
    transfer.frame.extended_can_id = extended_can_id;
    transfer.frame.payload_size = size;
    transfer.frame.payload = arr;
    return transport_can_driver.transmit(&transfer);
}
auto MY_MALLOC(std::size_t amount) -> void* {
    return o1heapAllocate(my_allocator, amount);
}
auto MY_FREE(void* pointer) -> void {
    o1heapFree(my_allocator, pointer);
}
std::chrono::microseconds GET_TIME_SINCE_BOOT() {
    return (std::chrono::microseconds)(HAL_GetTick() * 1000);
}
auto kocherga::getRandomByte() -> std::uint8_t {
    return static_cast<std::uint8_t>(
        std::rand() * std::numeric_limits<std::uint8_t>::max() / RAND_MAX
    );
}
void WAIT_FOR_EVENT() {
    HAL_Delay(1);
}

MyCANDriver::MyCANDriver() : tx_queue_{&MY_MALLOC, &MY_FREE} {};

auto MyCANDriver::configure(const Bitrate&                         bitrate,
                   const bool                                      silent,
                   const kocherga::can::CANAcceptanceFilterConfig& filter)
    -> std::optional<Mode>
{
    (void)bitrate;
    (void)silent;
    (void)filter;

    init();
    return Mode::Classic;
}

void MyCANDriver::init()
{
    if (!is_configured) {
        my_allocator = o1heapInit(base, cyphal::HEAP_SIZE);
        tx_queue_.clear();
        transport_can_driver.init(1000000, 0);
        is_configured = true;
    }
}

auto MyCANDriver::push(const bool          force_classic_can,
            const std::uint32_t extended_can_id,
            const std::uint8_t  payload_size,
            const void* const   payload) -> bool
{
    const std::chrono::microseconds now = GET_TIME_SINCE_BOOT();
    const bool ok = tx_queue_.push(now, force_classic_can, extended_can_id, payload_size, payload);
    pollTxQueue(now);
    return ok;
}

auto MyCANDriver::pop(PayloadBuffer& payload_buffer)
    -> std::optional<std::pair<std::uint32_t, std::uint8_t>>
{
    // originally it was without arguments...
    pollTxQueue(GET_TIME_SINCE_BOOT());
    auto res = CAN_POP(payload_buffer.data());
    return res;
}

void MyCANDriver::pollTxQueue(const std::chrono::microseconds now)
{
    if (const auto* const item = tx_queue_.peek()) {
        const bool expired = now > (item->timestamp + kocherga::can::SendTimeout);
        if (expired || CAN_PUSH(item->extended_can_id, item->payload_size, item->payload)) {
            tx_queue_.pop();
        }
    }
}
