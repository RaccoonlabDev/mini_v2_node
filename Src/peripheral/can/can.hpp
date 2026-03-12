/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_CAN_CAN_HPP_
#define SRC_PERIPHERAL_CAN_CAN_HPP_

#include <stdint.h>
#include <variant>

namespace HAL {

class Can {
public:
    enum class Instance {
        INSTANCE_1 = 0,
        INSTANCE_2,
    };

    enum class ErrorCode : int16_t {
        SUCCESS = 0,
        TIMEOUT_OR_FIFO_EMPTY = -1,
        INITIALIZATION_FAILED = -2,
        TRANSMISSION_ERROR = -3,
        RECEPTION_ERROR = -4,
        UNKNOWN_ERROR = -100,
    };

    struct InitConfig {
        uint32_t baud_rate;
    };

    struct ClassicFrame {
        uint32_t id;
        uint8_t data[8];
        uint8_t data_len;
        uint64_t timestamp_ns;
    };

    struct CanBusStats {
        uint32_t tx_frames;
        uint32_t rx_frames;
        uint32_t errors;
        uint32_t rx_overflow;
    };

    /**
     * @brief Binds this object to a concrete CAN peripheral instance.
     */
    explicit Can(Instance instance);

    /**
     * @brief Initializes selected CAN instance with requested parameters.
     * @return SUCCESS on success, otherwise an initialization-related error code.
     */
    [[nodiscard]] ErrorCode init(const InitConfig& config);

    /**
     * @brief Sends one classic CAN frame.
     * @return SUCCESS if frame was queued for TX, otherwise an error code.
     */
    [[nodiscard]] ErrorCode send(const ClassicFrame& frame);

    /**
     * @brief Receives one frame from RX FIFO when available.
     * @return ClassicFrame on success, or ErrorCode
     *         (e.g. TIMEOUT_OR_FIFO_EMPTY) for caller-side branching.
     */
    [[nodiscard]] std::variant<ClassicFrame, ErrorCode> receive();

    /**
     * @brief Returns lightweight transport counters collected by this instance.
     */
    [[nodiscard]] CanBusStats getTransportStats() const {
        return _stats;
    }

private:
    Instance _instance;
    bool _is_initialized = false;
    CanBusStats _stats{0, 0, 0, 0};
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_CAN_CAN_HPP_
