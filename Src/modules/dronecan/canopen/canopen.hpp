/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#ifndef SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_
#define SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_

#include "common/logging.hpp"
#include "libcanopen/canopen.hpp"
#include "module.hpp"

class CanopenModule : public Module {
public:
    CanopenModule() : Module(10.0F, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;

private:
    static constexpr uint32_t CANOPEN_BITRATE = 250000U;
    static constexpr uint8_t PIHER_NODE_ID = 0x7FU;
    static constexpr uint16_t PIHER_TPDO_ID = 0x1FFU;
    static constexpr uint8_t PIHER_TPDO_DLC = 8U;
    static constexpr uint8_t MAX_RX_FRAMES_PER_SPIN = 8U;
    static constexpr uint8_t MAX_NMT_ATTEMPTS = 5U;
    static constexpr uint32_t NMT_RETRY_INTERVAL_MS = 700U;

    libcanopen::Node* _node{nullptr};
    uint32_t _last_nmt_ms{0U};
    uint8_t _nmt_attempts{0U};
    bool _reported_first_tpdo{false};
    Logging _logger{"canopen"};
};

#endif  // SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_
