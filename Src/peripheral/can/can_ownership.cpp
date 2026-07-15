#include "peripheral/can/can_ownership.hpp"

#include <stddef.h>

namespace HAL {

namespace {

struct Claim {
    const void* handle;
    CanOwner owner;
};

constexpr size_t MAX_CAN_PERIPHERALS = 3U;
Claim claims[MAX_CAN_PERIPHERALS]{};
size_t claim_count = 0U;

}  // namespace

bool claimCanPeripheral(const void* handle, const CanOwner owner) {
    if (handle == nullptr) {
        return false;
    }
    for (size_t idx = 0U; idx < claim_count; idx++) {
        if (claims[idx].handle == handle) {
            return claims[idx].owner == owner;
        }
    }
    if (claim_count >= MAX_CAN_PERIPHERALS) {
        return false;
    }
    claims[claim_count] = {.handle = handle, .owner = owner};
    claim_count++;
    return true;
}

}  // namespace HAL
