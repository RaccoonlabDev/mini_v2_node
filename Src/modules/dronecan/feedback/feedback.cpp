/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "feedback.hpp"
#include "peripheral/adc/circuit_periphery.hpp"


REGISTER_MODULE(FeedbackModule)


void FeedbackModule::init() {
    mode = Module::Mode::STANDBY;
}

void FeedbackModule::update_params() {
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

void FeedbackModule::spin_once() {
    if (feedback_type == FeedbackType::DISABLED) {
        return;
    }

    for (auto& pwm : PWMModule::params) {
        if (pwm.channel < 0) {
            continue;
        }

        switch (cmd_type) {
            case CommandType::RAW_COMMAND:
                publish_esc_status(pwm);
                break;
            case CommandType::ARRAY_COMMAND:
                publish_actuator_status(pwm);
                break;
            case CommandType::HARDPOINT_COMMAND:
                publish_hardpoint_status(pwm);
                break;
            default:
                break;
        }
    }
}

void FeedbackModule::publish_esc_status(PwmChannelInfo& pwm) {
    esc_status.msg = {
        .error_count = esc_status.msg.error_count + 1,
        .voltage = CircuitPeriphery::voltage_vin(),
        .current = CircuitPeriphery::current(),
        .temperature = static_cast<float>(CircuitPeriphery::temperature()),
        .rpm = 0,
        .power_rating_pct = HAL::Pwm::get_percent(pwm.pin, pwm.min, pwm.max),
        .esc_index = static_cast<uint8_t>(pwm.channel),
    };

    esc_status.publish();
}

void FeedbackModule::publish_actuator_status(PwmChannelInfo& pwm) {
    actuator_status.msg = {
        .actuator_id = static_cast<uint8_t>(pwm.channel),

        // The following fields are not used in PX4 anyway
        // Let's fill them with something useful for logging for a while
        .position = CircuitPeriphery::voltage_5v(),
        .force = CircuitPeriphery::current(),
        .speed = static_cast<float>(CircuitPeriphery::temperature()),

        .reserved = 0,
        .power_rating_pct = HAL::Pwm::get_percent(pwm.pin, pwm.min, pwm.max),
    };

    actuator_status.publish();
}

void FeedbackModule::publish_hardpoint_status(PwmChannelInfo& pwm) {
    static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
    static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

    auto pwm_duration_us = HAL::Pwm::get_duration(pwm.pin);

    hardpoint_status.msg = {
        .hardpoint_id = static_cast<uint8_t>(pwm.channel),
        .payload_weight = 0.0f,
        .payload_weight_variance = 0.0f,
        .status = (pwm_duration_us == pwm.min) ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX,
    };

    hardpoint_status.publish();
}
