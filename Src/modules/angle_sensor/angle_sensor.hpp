/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#ifndef SRC_MODULES_ANGLE_SENSOR_ANGLE_SENSOR_HPP_
#define SRC_MODULES_ANGLE_SENSOR_ANGLE_SENSOR_HPP_

#include "common/logging.hpp"
#include "libcanopen/canopen.hpp"
#include "libcanopen/lss.hpp"
#include "module.hpp"

class AngleSensorModule : public Module {
public:
    AngleSensorModule() : Module(10.0F, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;

private:
    static constexpr uint8_t PIHER_OLD_NODE_ID = 25U;
    static constexpr uint8_t PIHER_NEW_NODE_ID = 0x7F;
    static constexpr uint16_t TPDO1_BASE_ID = 0x180U;
    static constexpr uint8_t PIHER_TPDO_DLC = 8U;
    static constexpr uint8_t MAX_RX_FRAMES_PER_SPIN = 8U;
    static constexpr uint8_t MAX_NMT_ATTEMPTS = 5U;
    static constexpr uint32_t NMT_RETRY_INTERVAL_MS = 700U;
    static constexpr uint32_t LOG_INTERVAL_MS = 1000U;
    static constexpr uint16_t ANGLE_RAW_MAX = 4095U;

    bool sendNmtStart();
    bool configureNodeIdWithLss();
    bool updateAngle();
    void logLssResult(const char* operation, const libcanopen::LssResult& result) const;
    void logActivation(const libcanopen::Frame& frame) const;
    void logAngle() const;

    libcanopen::Node* _node{nullptr};
    libcanopen::LssMaster* _lss_master{nullptr};
    uint32_t _last_nmt_ms{0U};
    uint32_t _last_log_ms{0U};
    uint32_t _angle_millidegrees{0U};
    uint8_t _nmt_attempts{0U};
    uint8_t _node_id{PIHER_OLD_NODE_ID};
    uint16_t _tpdo_id{TPDO1_BASE_ID + PIHER_OLD_NODE_ID};
    bool _active{false};
    bool _lss_configuration_ok{false};
    Logging _logger{"ANGLE"};
};

#endif  // SRC_MODULES_ANGLE_SENSOR_ANGLE_SENSOR_HPP_
