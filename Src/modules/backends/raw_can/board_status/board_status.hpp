/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_BACKENDS_RAW_CAN_BOARD_STATUS_BOARD_STATUS_HPP_
#define SRC_MODULES_BACKENDS_RAW_CAN_BOARD_STATUS_BOARD_STATUS_HPP_

#include "common/datapool.hpp"
#include "common/module.hpp"
#include "topics/board_status.hpp"

/**
 * @brief Example frontend: puts topics::BoardStatusTopic on CAN as a plain frame of a custom
 * protocol, see frame.hpp. No release target includes it; rl_sitl_raw_can runs it.
 *
 * It shares the bus with DroneCAN: 11-bit ids can't be taken for DroneCAN, which uses 29-bit ones.
 * It only transmits, and leaves the health and the VSSC bit to the DroneCAN frontend.
 */
class RawCanBoardStatus : public Module {
public:
    // The CAN driver comes from libdcnode, and the DroneCAN core initializes it.
    RawCanBoardStatus() : Module(20.0F, Protocol::DRONECAN, VSSC_BIT_NONE) {}

protected:
    void update_params() override;
    void spin_once() override;

private:
    datapool::Subscriber<topics::BoardStatusTopic> status_sub;
    uint8_t node_id{0};
};

#endif  // SRC_MODULES_BACKENDS_RAW_CAN_BOARD_STATUS_BOARD_STATUS_HPP_
