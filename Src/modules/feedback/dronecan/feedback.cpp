/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "feedback.hpp"
#include "peripheral/adc/circuit_periphery.hpp"


REGISTER_MODULE(DronecanFeedbackModule)


void DronecanFeedbackModule::init() {
    mode = Module::Mode::STANDBY;
}

void DronecanFeedbackModule::update_params() {
    auto cmd_type_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TYPE);
    cmd_type = static_cast<CommandType>(cmd_type_value);

    auto feedback_type_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_FEEDBACK_TYPE);
    feedback_type = static_cast<FeedbackType>(feedback_type_value);

    switch (feedback_type) {
        case FeedbackType::DEFAULT_1_HZ:
            period_ms = 1000;
            break;
        case FeedbackType::DEFAULT_10_HZ:
            period_ms = 100;
            break;
        default:
            period_ms = 1000;
            break;
    }
}

void DronecanFeedbackModule::spin_once() {
    if (feedback_type == FeedbackType::DISABLED) {
        return;
    }

    for (uint_fast8_t pin_idx = 0; pin_idx < PWMModule::get_pins_amount(); pin_idx++) {
        if (!PWMModule::is_pin_enabled(pin_idx)) {
            continue;
        }

        switch (cmd_type) {
            case CommandType::RAW_COMMAND:
                publish_esc_status(pin_idx);
                break;
            case CommandType::ARRAY_COMMAND:
                publish_actuator_status(pin_idx);
                break;
            case CommandType::HARDPOINT_COMMAND:
                publish_hardpoint_status(pin_idx);
                break;
            default:
                break;
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
        .power_rating_pct = PWMModule::get_pin_percent(pin_idx),
        .esc_index = (uint8_t)PWMModule::get_pin_channel(pin_idx),
    };

    esc_status.publish();
}

void DronecanFeedbackModule::publish_actuator_status(uint8_t pin_idx) {
    actuator_status.msg = {
        .actuator_id = (uint8_t)PWMModule::get_pin_channel(pin_idx),

        // The following fields are not used in PX4 anyway
        // Let's fill them with something useful for logging for a while
        .position = CircuitPeriphery::voltage_5v(),
        .force = CircuitPeriphery::current(),
        .speed = static_cast<float>(CircuitPeriphery::temperature()),

        .reserved = 0,
        .power_rating_pct = PWMModule::get_pin_percent(pin_idx),
    };

    actuator_status.publish();
}

void DronecanFeedbackModule::publish_hardpoint_status(uint8_t pin_idx) {
    static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
    static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

    hardpoint_status.msg = {
        .hardpoint_id = (uint8_t)PWMModule::get_pin_channel(pin_idx),
        .payload_weight = 0.0f,
        .payload_weight_variance = 0.0f,
        .status = PWMModule::get_pin_percent(pin_idx) == 0 ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX,
    };

    hardpoint_status.publish();
}
