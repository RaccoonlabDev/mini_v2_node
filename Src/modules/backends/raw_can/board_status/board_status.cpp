/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "board_status.hpp"
#include <algorithm>
#include "frame.hpp"
#include "libdcnode/can_driver.h"
#include "params.hpp"

REGISTER_MODULE(RawCanBoardStatus)

void RawCanBoardStatus::update_params() {
    node_id = static_cast<uint8_t>(paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID));
}

void RawCanBoardStatus::spin_once() {
    topics::BoardStatus status;
    if (!status_sub.update(&status)) {
        return;
    }

    const auto data = raw_can::encode_board_status(status);
    CanardCANFrame frame{};
    frame.id = raw_can::BOARD_STATUS_BASE_ID + node_id;  // no CANARD_CAN_FRAME_EFF: an 11-bit id
    std::ranges::copy(data, frame.data);
    frame.data_len = raw_can::FRAME_SIZE;
    canDriverTransmit(&frame, CAN_DRIVER_FIRST);
}
