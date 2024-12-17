/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "canopen.hpp"
#include "can_driver.h"
#include "common/algorithms.hpp"

REGISTER_MODULE(CanopenModule)

void CanopenModule::init() {
    set_health(Status::OK);

    if (canDriverInit(1000000, CAN_DRIVER_SECOND) < 0) {
        set_health(Status::FATAL_MALFANCTION);
    }

    if (raw_command_sub.init(raw_command_cb) < 0) {
        set_health(Status::FATAL_MALFANCTION);
    }

    set_mode(Mode::STANDBY);
}

void CanopenModule::spin_once() {
    // We probably want to check TTL here
}

void CanopenModule::raw_command_cb(const RawCommand_t& msg) {
    if (msg.size < RAW_COMMAND_CHANNEL + 1) {
        return;
    }

    auto raw_command_value = msg.raw_cmd[RAW_COMMAND_CHANNEL];
    uint8_t setpoint_percent = mapFloat((float)raw_command_value, 0.0f, 8191, 0.0f, 100.0f);

    CanardCANFrame frame;
    frame.id = RAW_COMMAND_CHANNEL;
    frame.data[0] = setpoint_percent;
    frame.data_len = 1;

    canDriverTransmit(&frame, CAN_DRIVER_SECOND);
}
