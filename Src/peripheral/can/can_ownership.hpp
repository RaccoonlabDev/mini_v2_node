#ifndef SRC_PERIPHERAL_CAN_CAN_OWNERSHIP_HPP_
#define SRC_PERIPHERAL_CAN_CAN_OWNERSHIP_HPP_

#include <stdint.h>

namespace HAL {

enum class CanOwner : uint8_t {
    DRONECAN = 0,
    CANOPEN,
    CYPHAL,
};

[[nodiscard]] bool claimCanPeripheral(const void* handle, CanOwner owner);

}  // namespace HAL

#endif  // SRC_PERIPHERAL_CAN_CAN_OWNERSHIP_HPP_
