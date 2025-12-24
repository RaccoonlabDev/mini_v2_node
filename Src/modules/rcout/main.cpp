/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/rcout/main.hpp"
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

REGISTER_MODULE(RcoutModule)

template <typename Fn>
static inline void for_active_frontend(Fn&& fn) {
    auto proto = ModuleManager::get_active_protocol();
#if CONFIG_USE_DRONECAN == 1
    if (proto == Module::Protocol::DRONECAN) fn(dronecan_frontend);
#endif
#if CONFIG_USE_CYPHAL == 1
    if (proto == Module::Protocol::CYPHAL) fn(cyphal_frontend);
#endif
}

void RcoutModule::init() {
    Driver::RCPWM::init();
    logger.log_debug("init");
    for_active_frontend([this](auto& fe) { fe.init(); });
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
void RcoutModule::spin_once() {
    bool at_least_one_channel_is_engaged = false;
    bool at_least_one_ttl_detected = false;

    for (auto&& [rcout, timing] : zip(Driver::RCPWM::channels, RcoutModule::timings)) {
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

        rcout.set_default();
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

    static uint32_t last_gimbal_pub_ms = 0;
    uint32_t now = HAL_GetTick();
    if (now - last_gimbal_pub_ms >= 1000) {
        last_gimbal_pub_ms = now;
        #if CONFIG_USE_DRONECAN == 1
        if (ModuleManager::get_active_protocol() == Protocol::DRONECAN) {
            dronecan_frontend.publish_gimbal_status();
        }
        #endif
    }
}

void RcoutModule::update_params() {
    cmd_ttl = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);
    for (auto& timing : timings) {
        timing.set_cmd_ttl(cmd_ttl);
    }
    servo_coefficient = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_INPUT_TYPE);
    auto param_frequency = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    auto frequency = static_cast<uint16_t>(param_frequency);
    Driver::RCPWM::set_frequency(frequency);

    for_active_frontend([](auto& fe) { fe.update_params(); });
    Driver::RCPWM::update_params();
}
