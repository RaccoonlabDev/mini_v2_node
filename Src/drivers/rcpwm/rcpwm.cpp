/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "rcpwm.hpp"
#include <array>
#include "params.hpp"
#include "modules/pwm/main.hpp"


#define CH(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF

std::array<Driver::RcpwmChannel, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> Driver::RCPWM::channels = {{
    {{.min = MIN(1), .max = MAX(1), .def = DEF(1), .ch = CH(1)}, HAL::PwmPin::PWM_1},
    {{.min = MIN(2), .max = MAX(2), .def = DEF(2), .ch = CH(2)}, HAL::PwmPin::PWM_2},
    {{.min = MIN(3), .max = MAX(3), .def = DEF(3), .ch = CH(3)}, HAL::PwmPin::PWM_3},
    {{.min = MIN(4), .max = MAX(4), .def = DEF(4), .ch = CH(4)}, HAL::PwmPin::PWM_4},
}};

int8_t Driver::RCPWM::init() {
    for (auto param : channels) {
        HAL::Pwm::init(param.pin);
    }

    return 0;
}

void Driver::RCPWM::update_params() {
    for (auto& pwm : channels) {
        pwm.channel = paramsGetIntegerValue(pwm.names.ch);
        pwm.def = paramsGetIntegerValue(pwm.names.def);
        pwm.min = paramsGetIntegerValue(pwm.names.min);
        pwm.max = paramsGetIntegerValue(pwm.names.max);
    }
}

void Driver::RCPWM::set_frequency(uint16_t frequency) {
    if (HAL::Pwm::get_frequency(channels[0].pin) != frequency) {
        for (const auto& pwm : channels) {
            HAL::Pwm::set_frequency(pwm.pin, frequency);
        }
    }
}

uint8_t Driver::RCPWM::get_pin_percent(uint8_t pin_idx) {
    if (!is_pin_enabled(pin_idx)) {
        return 0;
    }

    return HAL::Pwm::get_percent(channels[pin_idx].pin, channels[pin_idx].min, channels[pin_idx].max);
}

int8_t Driver::RCPWM::get_pin_channel(uint8_t pin_idx) {
    return pin_idx < get_pins_amount() ? channels[pin_idx].channel : -1;
}

bool Driver::RCPWM::is_pin_enabled(uint8_t pin_idx) {
    return get_pin_channel(pin_idx) >= 0;
}

void Driver::RcpwmChannel::set_us(uint16_t duration_us) const {
    HAL::Pwm::set_duration(pin, duration_us);
}

void Driver::RcpwmChannel::set_percent(uint8_t percent) {
    if (percent == 0) {
        set_us(min);
    } else if (percent >= 100) {
        set_us(max);
    } else {
        set_normalized_unsigned(0.01 * percent);
    }
}

void Driver::RcpwmChannel::set_normalized_signed(float normalized_signed_value) const {
    normalized_signed_value = std::clamp(normalized_signed_value, -1.0f, +1.0f);
    set_us(mapFloatCommandToPwm(normalized_signed_value, min, max, def));
}

void Driver::RcpwmChannel::set_normalized_unsigned(float normalized_unsigned_value) const {
    normalized_unsigned_value = std::clamp(normalized_unsigned_value, 0.0f, 1.0f);
    set_us((uint16_t)mapFloat(normalized_unsigned_value, 0.0f, +1.0f, min, max));
}

void Driver::RcpwmChannel::set_int14(uint16_t cmd_int14) const {
    set_us(mapInt16CommandToPwm(cmd_int14, min, max, def));
}

void Driver::RcpwmChannel::set_default() const {
    set_us(def);
}
