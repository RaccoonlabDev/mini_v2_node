/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "angle_sensor.hpp"

#include <stdio.h>

#include "modules/dronecan/canopen/canopen.hpp"

REGISTER_MODULE(AngleSensorModule)

void AngleSensorModule::init() {
    _node = CanopenModule::getNode();
    if (_node == nullptr) {
        set_health(Status::FATAL_MALFANCTION);
        set_mode(Mode::STANDBY);
        return;
    }

    set_mode(Mode::INITIALIZATION);
    (void)sendNmtStart();
}

bool AngleSensorModule::sendNmtStart() {
    _nmt_attempts++;
    _last_nmt_ms = HAL_GetTick();
    if (_node->sendNmtStart(PIHER_NODE_ID) < 0) {
        set_health(Status::MAJOR_FAILURE);
        return false;
    }
    if (_nmt_attempts == 1U) {
        _logger.log_info("PIHER NMT start sent: 000#017F");
    }
    return true;
}

bool AngleSensorModule::updateAngle() {
    libcanopen::Frame frame{};
    if (!_node->getLastTpdo(frame)) {
        return false;
    }

    _active = true;
    const uint16_t raw = static_cast<uint16_t>((static_cast<uint16_t>(frame.data[0]) << 8U) |
                                                frame.data[1]);
    if (raw > ANGLE_RAW_MAX) {
        set_health(Status::MINOR_FAILURE);
        return false;
    }

    _angle_millidegrees = static_cast<uint32_t>(raw) * 360000U / ANGLE_RAW_MAX;
    if (_last_log_ms == 0U) {
        logActivation(frame);
        _last_log_ms = HAL_GetTick();
    }
    set_health(Status::OK);
    set_mode(Mode::STANDBY);
    return true;
}

void AngleSensorModule::logActivation(const libcanopen::Frame& frame) const {
    char message[128]{};
    (void)snprintf(message,
                   sizeof(message),
                   "PIHER active: %03X#%02X%02X%02X%02X%02X%02X%02X%02X angle=%lu.%03lu deg",
                   frame.id,
                   frame.data[0],
                   frame.data[1],
                   frame.data[2],
                   frame.data[3],
                   frame.data[4],
                   frame.data[5],
                   frame.data[6],
                   frame.data[7],
                   static_cast<uint64_t>(_angle_millidegrees / 1000U),
                   static_cast<uint64_t>(_angle_millidegrees % 1000U));
    _logger.log_info(message);
}

void AngleSensorModule::logAngle() const {
    char message[48]{};
    (void)snprintf(message,
                   sizeof(message),
                   "PIHER angle=%lu.%03lu deg",
                   static_cast<uint64_t>(_angle_millidegrees / 1000U),
                   static_cast<uint64_t>(_angle_millidegrees % 1000U));
    _logger.log_info(message);
}

void AngleSensorModule::spin_once() {
    if (_node == nullptr) {
        return;
    }

    const uint32_t now_ms = HAL_GetTick();
    if (!_active && _nmt_attempts < MAX_NMT_ATTEMPTS &&
        now_ms - _last_nmt_ms >= NMT_RETRY_INTERVAL_MS) {
        (void)sendNmtStart();
    }

    const int16_t received =
        _node->spinOnce(PIHER_TPDO_ID, PIHER_TPDO_DLC, MAX_RX_FRAMES_PER_SPIN);
    if (received < 0) {
        set_health(Status::MAJOR_FAILURE);
        return;
    }
    if (received > 0) {
        (void)updateAngle();
    } else if (!_active && _nmt_attempts >= MAX_NMT_ATTEMPTS &&
               now_ms - _last_nmt_ms >= NMT_RETRY_INTERVAL_MS) {
        set_health(Status::MAJOR_FAILURE);
    }

    if (_active && _last_log_ms != 0U && now_ms - _last_log_ms >= LOG_INTERVAL_MS) {
        logAngle();
        _last_log_ms = now_ms;
    }
}
