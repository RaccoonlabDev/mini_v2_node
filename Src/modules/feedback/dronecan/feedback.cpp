/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "feedback.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "peripheral/adc/circuit_periphery.hpp"
#include "drivers/rcpwm/rcpwm.hpp"
#include "params.hpp"

REGISTER_MODULE(DronecanFeedbackModule)


void DronecanFeedbackModule::init() {
    set_mode(Mode::STANDBY);
}

void DronecanFeedbackModule::update_params() {
    feedback_esc_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_ESC_ENABLE));
    feedback_actuator_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_ACTUATOR_ENABLE));
    feedback_hardpoint_enabled = static_cast<bool>(paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_HARDPOINT_ENABLE));
}

void DronecanFeedbackModule::spin_once() {
    for (uint_fast8_t pin_idx = 0; pin_idx < Driver::RCPWM::get_pins_amount(); pin_idx++) {
        if (!Driver::RCPWM::is_pin_enabled(pin_idx)) {
            continue;
        }

        if (feedback_esc_enabled) {
            publish_esc_status(pin_idx);
        }

        if (feedback_actuator_enabled) {
            publish_actuator_status(pin_idx);
        }

        if (feedback_hardpoint_enabled) {
            publish_hardpoint_status(pin_idx);
        }
    }
}

void DronecanFeedbackModule::publish_esc_status(uint8_t pin_idx) {
    esc_status.msg = {
        .error_count = esc_status.msg.error_count + 1,
        .voltage = CircuitPeriphery::voltage_vin(),
        .current = CircuitPeriphery::current(),
        .temperature = static_cast<float>(CircuitPeriphery::temperature()),
        .rpm = 0,
        .power_rating_pct = Driver::RCPWM::get_pin_percent(pin_idx),
        .esc_index = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),
    };

    esc_status.publish();
}

void DronecanFeedbackModule::publish_actuator_status(uint8_t pin_idx) {
    actuator_status.msg = {
        .actuator_id = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),

        // The following fields are not used in PX4 anyway
        // Let's fill them with something useful for logging for a while
        .position = CircuitPeriphery::voltage_5v(),
        .force = CircuitPeriphery::current(),
        .speed = static_cast<float>(CircuitPeriphery::temperature()),

        .reserved = 0,
        .power_rating_pct = Driver::RCPWM::get_pin_percent(pin_idx),
    };

    actuator_status.publish();
}

void DronecanFeedbackModule::publish_hardpoint_status(uint8_t pin_idx) {
    static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
    static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

    hardpoint_status.msg = {
        .hardpoint_id = (uint8_t)Driver::RCPWM::get_pin_channel(pin_idx),
        .payload_weight = 0.0f,
        .payload_weight_variance = 0.0f,
        .status = Driver::RCPWM::get_pin_percent(pin_idx) == 0 ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX,
    };

    hardpoint_status.publish();
}
