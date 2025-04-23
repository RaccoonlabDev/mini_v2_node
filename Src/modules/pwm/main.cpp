/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/pwm/main.hpp"
#include <limits>
#include "common/algorithms.hpp"
#include "common/zip.hpp"

#ifndef CONFIG_USE_DRONECAN
#define CONFIG_USE_DRONECAN 0
#endif

#ifndef CONFIG_USE_CYPHAL
#define CONFIG_USE_CYPHAL 0
#endif

#if CONFIG_USE_DRONECAN == 1
    #include "dronecan_frontend/dronecan_frontend.hpp"
    static inline DronecanPwmFrontend dronecan_frontend;
#endif  // CONFIG_USE_DRONECAN

#if CONFIG_USE_CYPHAL == 1
    #include "cyphal_frontend/cyphal_frontend.hpp"
    static inline CyphalPwmFrontend cyphal_frontend;
#endif  // CONFIG_USE_CYPHAL

REGISTER_MODULE(PWMModule)

void PWMModule::init() {
    Driver::RCPWM::init();
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
    bool at_least_one_ttl_detected = false;

    for (auto&& [pwm, timing] : zip(Driver::RCPWM::channels, PWMModule::timings)) {
        if (timing.is_engaged()) {
            at_least_one_channel_is_engaged = true;
            continue;
        }

        if (timing.is_command_fresh()) {
            continue;
        }

        if (!timing.is_sleeping()) {
            at_least_one_ttl_detected = true;
        }

        pwm.set_default();
        timing.set_sleep_state();
    }

    if (!at_least_one_channel_is_engaged && get_mode() == Mode::ENGAGED) {
        if (at_least_one_ttl_detected) {
            logger.log_warn("Sleep (TTL)");
        } else {
            logger.log_info("Disarmed");
        }
    }

    if (at_least_one_channel_is_engaged && get_mode() == Mode::STANDBY) {
        logger.log_info("Engaged");
    }

    set_mode(at_least_one_channel_is_engaged ? Mode::ENGAGED : Module::Mode::STANDBY);
}

void PWMModule::update_params() {
    cmd_ttl = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);
    for (auto& timing : timings) {
        timing.set_cmd_ttl(cmd_ttl);
    }

    auto param_frequency = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    auto frequency = static_cast<uint16_t>(param_frequency);
    Driver::RCPWM::set_frequency(frequency);

#if CONFIG_USE_DRONECAN == 1
    if (ModuleManager::get_active_protocol() == Protocol::DRONECAN) {
        dronecan_frontend.update_params();
    }
#endif  // CONFIG_USE_DRONECAN

    Driver::RCPWM::update_params();
}
