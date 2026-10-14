/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_BOARD_STATUS_BOARD_STATUS_HPP_
#define SRC_MODULES_BOARD_STATUS_BOARD_STATUS_HPP_

#include "common/datapool.hpp"
#include "common/module.hpp"
#include "topics/board_status.hpp"

/**
 * @brief Measures the board and publishes topics::BoardStatusTopic.
 * What reaches a bus, the health and the VSSC bit included, is up to the protocol frontends.
 */
class BoardStatusModule : public Module {
public:
    BoardStatusModule() : Module(2.0F, Protocol::ANY, VSSC_BIT_NONE) {}

protected:
    void spin_once() override;

private:
    datapool::Publisher<topics::BoardStatusTopic> status_pub;
};

#endif  // SRC_MODULES_BOARD_STATUS_BOARD_STATUS_HPP_
