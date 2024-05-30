/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
#define SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_

#include "module.hpp"
#include "cyphal.hpp"
#include "cyphal_subscribers.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class SetpointSubscriber: public cyphal::CyphalSubscriber {
public:
    SetpointSubscriber() : CyphalSubscriber(cyphal::Cyphal::get_instance(), 65535) {}
    int8_t init();
private:
    void callback(const cyphal::CanardRxTransfer& transfer) override;
};

class SetpointModule : public BaseModule {
public:
    SetpointModule() : BaseModule(50) {}
    void init() override;

protected:
    void spin_once() override;

private:
    SetpointSubscriber sub;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
