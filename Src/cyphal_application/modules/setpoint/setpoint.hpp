/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
#define SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "cyphal_subscribers.hpp"

class SetpointSubscriber: public cyphal::CyphalSubscriber {
public:
    explicit SetpointSubscriber(cyphal::Cyphal* driver);
    int8_t init();
private:
    void callback(const cyphal::CanardRxTransfer& transfer) override;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
