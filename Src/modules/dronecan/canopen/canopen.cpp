/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "canopen.hpp"

#include <stdio.h>

#include "peripheral/can/canopen_stm32h7.hpp"

REGISTER_MODULE(CanopenModule)

void CanopenModule::init() {
    static libcanopen::Node node(canopenGetTransportApi());
    _node = &node;
    if (_node->init(CANOPEN_BITRATE, PIHER_NODE_ID) < 0) {
        set_health(Status::FATAL_MALFANCTION);
    } else if (_node->sendNmtStart() < 0) {
        set_health(Status::MAJOR_FAILURE);
    } else {
        _nmt_attempts = 1U;
        _last_nmt_ms = HAL_GetTick();
        _logger.log_info("PIHER NMT start sent: 000#017F");
        set_health(Status::OK);
    }
    set_mode(Mode::STANDBY);
}

void CanopenModule::spin_once() {
    if (_node == nullptr) {
        return;
    }
    const uint32_t now_ms = HAL_GetTick();
    if (!_reported_first_tpdo && _nmt_attempts > 0U && _nmt_attempts < MAX_NMT_ATTEMPTS &&
        now_ms - _last_nmt_ms >= NMT_RETRY_INTERVAL_MS) {
        if (_node->sendNmtStart() < 0) {
            set_health(Status::MAJOR_FAILURE);
        } else {
            _nmt_attempts++;
            _last_nmt_ms = now_ms;
        }
    }
    const int16_t received =
        _node->spinOnce(PIHER_TPDO_ID, PIHER_TPDO_DLC, MAX_RX_FRAMES_PER_SPIN);
    if (received < 0) {
        set_health(Status::MAJOR_FAILURE);
        return;
    }
    if (received > 0) {
        set_health(Status::OK);
        if (!_reported_first_tpdo) {
            libcanopen::Frame frame{};
            if (_node->getLastTpdo(frame)) {
                char message[96]{};
                (void)snprintf(message,
                               sizeof(message),
                               "PIHER active: %03X#%02X%02X%02X%02X%02X%02X%02X%02X",
                               frame.id,
                               frame.data[0],
                               frame.data[1],
                               frame.data[2],
                               frame.data[3],
                               frame.data[4],
                               frame.data[5],
                               frame.data[6],
                               frame.data[7]);
                _logger.log_info(message);
            }
            _reported_first_tpdo = true;
        }
    }
}
