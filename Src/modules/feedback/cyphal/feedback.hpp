/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
#define SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_

#include "libcpnode/cyphal.hpp"
#include <array>
#include "modules/rcout/main.hpp"
#include "module.hpp"
#include "drivers/rcpwm/rcpwm.hpp"
#include "reg.udral.service.actuator.common.Feedback_0_1.h"
#include "zubax.telega.CompactFeedback_1_0.h"

class CyphalFeedbackModule : public Module {
public:
    CyphalFeedbackModule() : Module(10, Protocol::CYPHAL) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    std::array<libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<reg_udral_service_actuator_common_Feedback_0_1>>, Driver::RCPWM::get_pins_amount()> udral_feedbacks;
    std::array<libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<zubax_telega_CompactFeedback_1_0>>, Driver::RCPWM::get_pins_amount()> compact_feedbacks;
};

#endif  // SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
