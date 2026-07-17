/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "angle_sensor.hpp"

#include <stdio.h>

#include "modules/dronecan/canopen/canopen.hpp"

REGISTER_MODULE(AngleSensorModule)

namespace {

const char* lssStatusName(const libcanopen::LssStatus status) {
    switch (status) {
        case libcanopen::LssStatus::SUCCESS:
            return "OK";
        case libcanopen::LssStatus::INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";
        case libcanopen::LssStatus::TIMEOUT:
            return "TIMEOUT";
        case libcanopen::LssStatus::TRANSPORT_ERROR:
            return "TRANSPORT_ERROR";
        case libcanopen::LssStatus::UNSUPPORTED:
            return "UNSUPPORTED";
        case libcanopen::LssStatus::SERVER_REJECTED:
            return "SERVER_REJECTED";
        case libcanopen::LssStatus::INVALID_RESPONSE:
            return "INVALID_RESPONSE";
        default:
            return "UNKNOWN";
    }
}

}  // namespace

void AngleSensorModule::init() {
    _node = CanopenModule::getNode();
    _lss_master = CanopenModule::getLssMaster();
    if (_node == nullptr || _lss_master == nullptr) {
        set_health(Status::FATAL_MALFANCTION);
        set_mode(Mode::STANDBY);
        return;
    }

    set_mode(Mode::INITIALIZATION);
    _lss_configuration_ok = configureNodeIdWithLss();
    (void)sendNmtStart();
}

void AngleSensorModule::logLssResult(const char* const operation,
                                     const libcanopen::LssResult& result) const {
    char message[112]{};
    (void)snprintf(message,
                   sizeof(message),
                   "PIHER LSS %s: %s error=%u extension=%u",
                   operation,
                   lssStatusName(result.status),
                   result.error_code,
                   result.error_extension);
    if (result.status == libcanopen::LssStatus::SUCCESS) {
        _logger.log_info(message);
    } else {
        _logger.log_error(message);
    }
}

bool AngleSensorModule::configureNodeIdWithLss() {
    _logger.log_info("PIHER LSS test: global configuration, node 100 -> 127");
    const libcanopen::LssResult enter_result =
        _lss_master->switchStateGlobal(libcanopen::LssState::CONFIGURATION);
    logLssResult("enter configuration", enter_result);

    bool configuration_ok = enter_result.status == libcanopen::LssStatus::SUCCESS;
    if (configuration_ok) {
        const libcanopen::LssResult node_result =
            _lss_master->configureNodeId(PIHER_NEW_NODE_ID, 10000);
        logLssResult("configure node 127", node_result);
        configuration_ok = node_result.status == libcanopen::LssStatus::SUCCESS;
        if (configuration_ok) {
            _node_id = PIHER_NEW_NODE_ID;
            _tpdo_id = TPDO1_BASE_ID + PIHER_NEW_NODE_ID;
            const libcanopen::LssResult store_result = _lss_master->storeConfiguration();
            logLssResult("store configuration", store_result);
            configuration_ok = store_result.status == libcanopen::LssStatus::SUCCESS;
        }
    }

    const libcanopen::LssResult leave_result =
        _lss_master->switchStateGlobal(libcanopen::LssState::WAITING);
    logLssResult("enter waiting", leave_result);
    configuration_ok = configuration_ok &&
                       leave_result.status == libcanopen::LssStatus::SUCCESS;
    if (!configuration_ok) {
        set_health(Status::MAJOR_FAILURE);
    }
    return configuration_ok;
}

bool AngleSensorModule::sendNmtStart() {
    _nmt_attempts++;
    _last_nmt_ms = HAL_GetTick();
    if (_node->sendNmtStart(_node_id) < 0) {
        set_health(Status::MAJOR_FAILURE);
        return false;
    }
    if (_nmt_attempts == 1U) {
        char message[64]{};
        (void)snprintf(message,
                       sizeof(message),
                       "PIHER NMT start sent: 000#01%02X",
                       _node_id);
        _logger.log_info(message);
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
    set_health(_lss_configuration_ok ? Status::OK : Status::MAJOR_FAILURE);
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
        _node->spinOnce(_tpdo_id, PIHER_TPDO_DLC, MAX_RX_FRAMES_PER_SPIN);
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
