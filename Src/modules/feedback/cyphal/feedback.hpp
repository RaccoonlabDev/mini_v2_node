/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
#define SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_

#include "cyphalNode/cyphal.hpp"
#include <array>
#include "cyphalNode/udral/feedback.hpp"
#include "cyphalNode/zubax/compact_feedback.hpp"
#include "modules/pwm/main.hpp"
#include "module.hpp"

class CyphalFeedbackModule : public Module {
public:
    CyphalFeedbackModule() : Module(10, Protocol::CYPHAL) {}

protected:
    void update_params() override;
    void spin_once() override;

private:
    std::array<udral::FeedbackPublisher, PWMModule::get_pins_amount()> udral_feedbacks;
    std::array<zubax::telega::CompactFeedbackPublisher, PWMModule::get_pins_amount()> compact_feedbacks;
};

#endif  // SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
