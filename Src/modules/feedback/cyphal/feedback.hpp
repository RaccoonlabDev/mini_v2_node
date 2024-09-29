/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
#define SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_

#include "cyphalNode/cyphal.hpp"
#include "reg/udral/service/actuator/common/Feedback_0_1.h"
#include "module.hpp"

#ifdef __cplusplus
extern "C" {
#endif

struct FeedbackPublisher: public cyphal::CyphalPublisher {
    FeedbackPublisher() : CyphalPublisher(cyphal::Cyphal::get_instance(), 65535) {}
    int8_t init();
    void publish();

    reg_udral_service_actuator_common_Feedback_0_1 msg;
};

class CyphalFeedbackModule : public Module {
public:
    CyphalFeedbackModule() : Module(1, Protocol::CYPHAL) {}

protected:
    void update_params() override;
    void spin_once() override;

private:
    FeedbackPublisher pub;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
