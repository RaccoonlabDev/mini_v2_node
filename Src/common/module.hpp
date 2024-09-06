/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_MODULE_HPP_
#define SRC_COMMON_MODULE_HPP_

#include <span>
#include <cstdint>
#include <array>
#include "main.h"


class Module {
public:
    enum class Status: uint8_t {    // DroneCAN     Cyphal
        OK                  = 0,    // OK           NOMINAL
        MINOR_FAILURE       = 1,    // WARNING      ADVISORY
        MAJOR_FAILURE       = 2,    // ERROR        CAUTION
        FATAL_MALFANCTION   = 3,    // CRITICAL     WARNING
    };

    enum class Mode: uint8_t {
        STANDBY             = 0,    // Low-power state, waiting further commands or events
        ENGAGED             = 1,    // Fully active and operational
        INITIALIZATION      = 2,    // After startup
        MAINTENANCE         = 3,    // Calibration, self-test
    };

    enum class Protocol: uint8_t {
        UNKNOWN             = 0,    // Nor Cyphal or DroneCAN
        CYPHAL              = 1,    // Only Cyphal
        DRONECAN            = 2,    // Only DroneCAN
        CYPHAL_AND_DRONECAN = 3,    // Both Cyphal and DroneCAN
    };

    /**
     * @brief The constructor is responsible only for allocating resources and registering
     * the object with a manager or ModuleManager. It doesn't perform any heavyweight operations
     * or initialization that might depend on peripheral or communication protocols.
     * @param[in] frequency is how many times per second the module should be spinned
     */
    explicit Module(float frequency, Protocol proto = Protocol::UNKNOWN);

    /**
     * @brief The function performs more complex initialization tasks. For instance, init might
     * configure hardware peripherals or start communication protocols.
     * Before the initialization, the module is in the INITIALIZATION mode. Once the initialization
     * is successfully finished, it goes to the OPERATIONAL mode.
     * The initialization is managed by ModuleManager.
     */
    virtual void init();

    /**
     * @brief The function spins the module with a specified in the constructor frequency.
     * The processing is managed by ModuleManager.
     */
    void process();

    /**
     * @brief Module state getters
     */
    Status get_health() const;
    Mode get_mode() const;
    Protocol get_protocol() const;

protected:
    /**
     * @brief The function manages the parameters from the persistent storage
     * The function is called in process() when the parameters are updated.
     */
    virtual void update_params() {}

    /**
     * @brief The function is not blocking. It should be finished ASAP.
     */
    virtual void spin_once() = 0;

    static uint32_t period_ms_from_frequency(float frequency);

    Status health{Status::OK};
    Mode mode{Mode::INITIALIZATION};
    Protocol protocol;

    uint32_t period_ms;
    uint32_t next_spin_time_ms{0};
};


class ModuleManager {
public:
    static void register_module(Module* app_module);
    static void init(Module::Protocol proto);
    static void process();

    static Module::Status get_global_status();
    static Module::Mode get_global_mode();
    static uint8_t get_vssc();

private:
    static constexpr uint8_t MAX_MODULES_AMOUNT{10};
    static inline std::array<Module*, MAX_MODULES_AMOUNT> modules;
    static inline std::span<Module*> active_modules;
    static inline uint8_t modules_amount{0};
    static inline Module::Protocol protocol;
};

/**
 * @brief Register module
 * This macro defines a static instance of a Module child with a unique name in an anonymous
 * namespace. It ensures that each module instance is unique within its translation unit,
 * preventing naming collisions.
 * Once a module is registered, his pointer is added to ModuleManager where it is automatically
 * handled.
 */
#define UNIQUE_NAME_CONCATENATE(x, y) x##y
#define UNIQUE_NAME(x) UNIQUE_NAME_CONCATENATE(x, __COUNTER__)
#define REGISTER_MODULE(ModuleClass) \
namespace { \
    static ModuleClass UNIQUE_NAME(moduleInstance); \
}

#endif  // SRC_COMMON_MODULE_HPP_
