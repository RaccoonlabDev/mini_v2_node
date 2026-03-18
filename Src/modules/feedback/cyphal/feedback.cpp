/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "feedback.hpp"
#include <algorithm>
#include <array>
#include "libcpnode/cyphal.hpp"
#include "params.hpp"
#include "drivers/board_monitor/board_monitor.hpp"

REGISTER_MODULE(CyphalFeedbackModule)

namespace {

constexpr std::array<int, 4> UDRAL_FEEDBACK_PORT_PARAMS = {
    PARAM_PUB_FEEDBACK_1_ID,
    PARAM_PUB_FEEDBACK_2_ID,
    PARAM_PUB_FEEDBACK_3_ID,
    PARAM_PUB_FEEDBACK_4_ID,
};

constexpr std::array<int, 4> COMPACT_FEEDBACK_PORT_PARAMS = {
    PARAM_PUB_COMPACT_FEEDBACK_1_ID,
    PARAM_PUB_COMPACT_FEEDBACK_2_ID,
    PARAM_PUB_COMPACT_FEEDBACK_3_ID,
    PARAM_PUB_COMPACT_FEEDBACK_4_ID,
};

template <typename MessageType, size_t N>
void init_publishers(
    libcpnode::Cyphal* cyphal,
    const std::array<int, N>& port_params,
    std::array<libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<MessageType>>, N>& publishers
) {
    const auto count = std::min<size_t>(Driver::RCPWM::get_pins_count(), port_params.size());
    for (size_t i = 0; i < count; ++i) {
        publishers[i] = libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<MessageType>>(
            cyphal,
            static_cast<uint16_t>(paramsGetIntegerValue(port_params[i]))
        );
    }
}

template <typename PublisherType, size_t N>
void update_publisher_ports(std::array<PublisherType, N>& publishers, const std::array<int, N>& port_params) {
    const auto count = std::min<size_t>(Driver::RCPWM::get_pins_count(), port_params.size());
    for (size_t i = 0; i < count; ++i) {
        publishers[i].setPortId(static_cast<uint16_t>(paramsGetIntegerValue(port_params[i])));
    }
}

}  // namespace

void CyphalFeedbackModule::init() {
    set_mode(Mode::STANDBY);
    auto cyphal = libcpnode::Cyphal::get_instance();
    init_publishers(cyphal, UDRAL_FEEDBACK_PORT_PARAMS, udral_feedbacks);
    init_publishers(cyphal, COMPACT_FEEDBACK_PORT_PARAMS, compact_feedbacks);
}


void CyphalFeedbackModule::update_params() {
    update_publisher_ports(udral_feedbacks, UDRAL_FEEDBACK_PORT_PARAMS);
    update_publisher_ports(compact_feedbacks, COMPACT_FEEDBACK_PORT_PARAMS);
}

void CyphalFeedbackModule::spin_once() {
    const auto udral_count = std::min<size_t>(Driver::RCPWM::get_pins_count(), udral_feedbacks.size());
    for (size_t pin_idx = 0; pin_idx < udral_count; pin_idx++) {
        auto& udral_feedback = udral_feedbacks[pin_idx];
        if (!udral_feedback.isEnabled()) {
            continue;
        }

        udral_feedback.msg.heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
        udral_feedback.msg.heartbeat.readiness.value = reg_udral_service_common_Readiness_0_1_ENGAGED;
        udral_feedback.msg.demand_factor_pct = Driver::RCPWM::get_pin_percent(pin_idx);
        udral_feedback.publish();
    }

    const auto compact_count = std::min<size_t>(Driver::RCPWM::get_pins_count(), compact_feedbacks.size());
    for (size_t pin_idx = 0; pin_idx < compact_count; pin_idx++) {
        auto& compact_feedback = compact_feedbacks[pin_idx];
        if (!compact_feedback.isEnabled()) {
            continue;
        }

        // Here we also need to map these things correctly...
        compact_feedback.msg = {
            .dc_voltage = static_cast<uint16_t>(BoardMonitor::voltage_vin()),
            .dc_current = static_cast<int16_t>(BoardMonitor::current()),
            .phase_current_amplitude = static_cast<int16_t>(BoardMonitor::current()),
            .velocity = 0,
            .demand_factor_pct = (int8_t)Driver::RCPWM::get_pin_percent(pin_idx),
        };
        compact_feedback.publish();
    }
}
