/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_CYPHAL_BOARD_STATUS_BOARD_STATUS_HPP_
#define SRC_MODULES_CYPHAL_BOARD_STATUS_BOARD_STATUS_HPP_

#include "common/datapool.hpp"
#include "libcpnode/libcpnode.hpp"
#include "module.hpp"
#include "topics/board_status.hpp"
#include "uavcan/si/unit/temperature/Scalar_1_0.h"
#include "uavcan/si/unit/voltage/Scalar_1_0.h"

/**
 * @brief Puts topics::BoardStatusTopic on Cyphal and reports the board health.
 */
class CyphalBoardStatus : public Module {
public:
    // The topic comes at 2 Hz: poll faster, so every sample goes out once and soon.
    CyphalBoardStatus() : Module(20.0F, Protocol::CYPHAL) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    datapool::Subscriber<topics::BoardStatusTopic> status_sub;

    template <typename Message>
    using Publisher = libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<Message>>;

    Publisher<uavcan_si_unit_voltage_Scalar_1_0> voltage_5v_pub;
    Publisher<uavcan_si_unit_voltage_Scalar_1_0> voltage_vin_pub;
    Publisher<uavcan_si_unit_temperature_Scalar_1_0> temperature_pub;
};

#endif  // SRC_MODULES_CYPHAL_BOARD_STATUS_BOARD_STATUS_HPP_
