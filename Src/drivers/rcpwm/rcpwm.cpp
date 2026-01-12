/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "rcpwm.hpp"
#include <array>
#include <algorithm>
#include "params.hpp"
#include "common/algorithms.hpp"

int8_t Driver::RCPWM::init() {
    for (const auto& param : channels) {
        HAL::Pwm::init(param.pin);
    }

    return 0;
}

void Driver::RCPWM::update_params() {
    for (auto& pwm : channels) {
        pwm.channel = paramsGetIntegerValue(pwm.idx.ch);
        pwm.def = paramsGetIntegerValue(pwm.idx.def);
        pwm.min = paramsGetIntegerValue(pwm.idx.min);
        pwm.max = paramsGetIntegerValue(pwm.idx.max);
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

    return HAL::Pwm::get_percent
        (channels[pin_idx].pin, channels[pin_idx].min, channels[pin_idx].max);
}

int16_t Driver::RCPWM::get_current_angle(uint16_t max_servo_angle, uint8_t pin_idx) {
    if (!is_pin_enabled(pin_idx)) return 0;

    auto current_us = HAL::Pwm::get_duration(channels[pin_idx].pin);
    float normalized = mapFloat(current_us,
                                channels[pin_idx].min,
                                channels[pin_idx].max,
                                -1.0f, +1.0f);

    float angle_deg = normalized * (max_servo_angle / 2.0f);
    return static_cast<int16_t>(angle_deg);
}

int8_t Driver::RCPWM::get_pin_channel(uint8_t pin_idx) {
    return pin_idx < get_pins_amount() ? channels[pin_idx].channel : -1;
}

bool Driver::RCPWM::is_pin_enabled(uint8_t pin_idx) {
    return get_pin_channel(pin_idx) >= 0;
}

void Driver::RcpwmChannel::set_percent(int8_t percent) const {
    if (percent < 0) {
        _set_us(def);
    } else if (percent == 0) {
        _set_us(min);
    } else if (percent >= 100) {
        _set_us(max);
    } else {
        auto clamped_percent = std::clamp((float)percent, 0.0f, 100.0f);
        auto pwm_duration_us = (uint16_t)mapFloat(clamped_percent, 0.0f, +100.0f, min, max);
        _set_us(pwm_duration_us);
    }
}

void Driver::RcpwmChannel::set_normalized_signed(float normalized_signed_value) const {
    normalized_signed_value = std::clamp(normalized_signed_value, -1.0f, +1.0f);
    _set_us(mapFloatCommandToPwm(normalized_signed_value, min, max, def));
}

void Driver::RcpwmChannel::set_normalized_unsigned(float normalized_unsigned_value) const {
    normalized_unsigned_value = std::clamp(normalized_unsigned_value, 0.0f, 1.0f);
    auto percent = static_cast<uint8_t>(100 * normalized_unsigned_value);
    set_percent(percent);
}

void Driver::RcpwmChannel::set_int14(uint16_t cmd_int14) const {
    _set_us(mapInt16CommandToPwm(cmd_int14, min, max, def));
}

void Driver::RcoutChannel::set_default() const {
    set_percent(-1);
}

void Driver::RcpwmChannel::_set_us(uint16_t duration_us) const {
    HAL::Pwm::set_duration(pin, duration_us);
}
