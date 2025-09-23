/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include "logging.hpp"
#include "module.hpp"
#include "params.hpp"

#ifndef CONFIG_USE_DRONECAN
#define CONFIG_USE_DRONECAN 0
#elif defined(CONFIG_USE_DRONECAN) && CONFIG_USE_DRONECAN == 1
#include "logger.hpp"
#endif

#ifndef CONFIG_USE_CYPHAL
#define CONFIG_USE_CYPHAL 0
#elif defined(CONFIG_USE_CYPHAL) && CONFIG_USE_CYPHAL == 1
#endif


Logging::Logging(const char* source_) : source(source_) {
}

void Logging::log(uint8_t severity, [[maybe_unused]] const char* text) const {
    if (severity < paramsGetIntegerValue(PARAM_LOG_LEVEL)) {
        return;
    }

#if CONFIG_USE_DRONECAN == 1
    if (ModuleManager::get_active_protocol() == Module::Protocol::DRONECAN) {
        DronecanLogger::log_global(severity, source, text);
    }
#endif

#if CONFIG_USE_CYPHAL == 1
    if (ModuleManager::get_active_protocol() == Module::Protocol::CYPHAL) {
        // CyphalLogger::log_global(severity, source, text);
    }
#endif
}

void Logging::log_debug(const char* text) const {
    log(0, text);
}

void Logging::log_info(const char* text) const {
    log(1, text);
}

void Logging::log_warn(const char* text) const {
    log(2, text);
}

void Logging::log_error(const char* text) const {
    log(3, text);
}
