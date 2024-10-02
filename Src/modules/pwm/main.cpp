/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/pwm/main.hpp"
#include <limits>
#include "common/algorithms.hpp"

#ifndef CONFIG_USE_DRONECAN
#define CONFIG_USE_DRONECAN 0
#endif

#ifndef CONFIG_USE_CYPHAL
#define CONFIG_USE_CYPHAL 0
#endif


#define CH(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF

#if CONFIG_USE_DRONECAN == 1
    #include "dronecan_frontend/dronecan_frontend.hpp"
    static inline DronecanPwmFrontend dronecan_frontend;
#endif  // CONFIG_USE_DRONECAN

#if CONFIG_USE_CYPHAL == 1
    #include "cyphal_frontend/cyphal_frontend.hpp"
    static inline CyphalPwmFrontend cyphal_frontend;
#endif  // CONFIG_USE_CYPHAL

REGISTER_MODULE(PWMModule)

std::array<PwmChannelInfo, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> PWMModule::params = {{
    {{.min = MIN(1), .max = MAX(1), .def = DEF(1), .ch = CH(1)}, HAL::PwmPin::PWM_1},
    {{.min = MIN(2), .max = MAX(2), .def = DEF(2), .ch = CH(2)}, HAL::PwmPin::PWM_2},
    {{.min = MIN(3), .max = MAX(3), .def = DEF(3), .ch = CH(3)}, HAL::PwmPin::PWM_3},
    {{.min = MIN(4), .max = MAX(4), .def = DEF(4), .ch = CH(4)}, HAL::PwmPin::PWM_4},
}};

void PWMModule::init() {
    for (auto param : params) {
        HAL::Pwm::init(param.pin);
    }
    logger.log_debug("init");

#if CONFIG_USE_DRONECAN == 1
    if (ModuleManager::get_active_protocol() == Protocol::DRONECAN) {
        dronecan_frontend.init(this);
    }
#endif  // CONFIG_USE_DRONECAN

#if CONFIG_USE_CYPHAL == 1
    if (ModuleManager::get_active_protocol() == Protocol::CYPHAL) {
        cyphal_frontend.init(this);
    }
#endif  // CONFIG_USE_CYPHAL
}

int8_t PWMModule::get_pin_channel(uint8_t pin_idx) {
    return pin_idx < PWMModule::get_pins_amount() ? params[pin_idx].channel : -1;
}

bool PWMModule::is_pin_enabled(uint8_t pin_idx) {
    return get_pin_channel(pin_idx) >= 0;
}

uint8_t PWMModule::get_pin_percent(uint8_t pin_idx) {
    if (!is_pin_enabled(pin_idx)) {
        return 0;
    }

    return HAL::Pwm::get_percent(params[pin_idx].pin, params[pin_idx].min, params[pin_idx].max);
}

/**
 * Control:
 * 1. Set default PWM for a channel if his command is outdated
 *
 * Logging:
 * 1. WARN message once TTL happend
 * 2. INFO message once Node is ENGAGED (armed)
 * 3. INFO message once Node is STANDBY (disarmed)
 */
void PWMModule::spin_once() {
    bool at_least_one_channel_is_engaged = false;
    bool ttl_detected = false;

    for (auto& pwm : params) {
        auto crnt_time_ms = HAL_GetTick();
        if (crnt_time_ms < pwm.engaged_deadline_ms) {
            at_least_one_channel_is_engaged = true;
            continue;
        }

        if (pwm.last_recv_time_ms != 0 && crnt_time_ms >= pwm.last_recv_time_ms + cmd_ttl) {
            ttl_detected = true;
        }

        if (crnt_time_ms >= pwm.last_recv_time_ms + cmd_ttl) {
            HAL::Pwm::set_duration(pwm.pin, pwm.def);
            pwm.last_recv_time_ms = 0;
            pwm.engaged_deadline_ms = 0;
        }
    }

    if (!at_least_one_channel_is_engaged && mode == Mode::ENGAGED) {
        if (ttl_detected) {
            logger.log_warn("TTL");
        } else {
            logger.log_info("Disarmed");
        }
    }

    if (at_least_one_channel_is_engaged && mode == Mode::STANDBY) {
        logger.log_info("Engaged");
    }

    mode = at_least_one_channel_is_engaged ? Mode::ENGAGED : Module::Mode::STANDBY;
}

void PWMModule::update_params() {
    cmd_ttl = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);

    // If frequency of any channel is not as required, then all channels need the update
    pwm_freq = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    if (HAL::Pwm::get_frequency(params[0].pin) != pwm_freq) {
        for (const auto& pwm : params) {
            HAL::Pwm::set_frequency(pwm.pin, pwm_freq);
        }
    }

#if CONFIG_USE_DRONECAN == 1
    if (ModuleManager::get_active_protocol() == Protocol::DRONECAN) {
        dronecan_frontend.update_params();
    }
#endif  // CONFIG_USE_DRONECAN

    for (auto& pwm : params) {
        pwm.channel = paramsGetIntegerValue(pwm.names.ch);
        pwm.def = paramsGetIntegerValue(pwm.names.def);
        pwm.min = paramsGetIntegerValue(pwm.names.min);
        pwm.max = paramsGetIntegerValue(pwm.names.max);
    }
}
