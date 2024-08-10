/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_MODULE_HPP_
#define SRC_COMMON_MODULE_HPP_

#include <cstdint>
#include <array>
#include <span>
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
        OPERATIONAL         = 0,    // After successful initialization
        INITIALIZATION      = 1,    // After startup
        MAINTENANCE         = 2,    // Calibration, self-test
    };

    /**
     * @brief The constructor is responsible only for allocating resources and registering
     * the object with a manager or ModuleManager. It doesn't perform any heavyweight operations
     * or initialization that might depend on peripheral or communication protocols.
     * @param[in] frequency is how many times per second the module should be spinned
     */
    explicit Module(float frequency);

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

protected:
    /**
     * @brief The function manages the parameters from the persistent storage
     * The function is called in process() when the parameters are updated.
     */
    virtual void update_params() {};

    /**
     * @brief The function is not blocking. It should be finished ASAP.
     */
    virtual void spin_once() = 0;

    static uint32_t period_ms_from_frequency(float frequency);

    Status health{Status::OK};
    Mode mode{Mode::INITIALIZATION};

    uint32_t period_ms;
    uint32_t next_spin_time_ms{0};
};


class ModuleManager {
public:
    static void register_module(Module* app_module);
    static void init();
    static void process();

    static Module::Status get_global_status();
    static Module::Mode get_global_mode();
    static uint8_t get_vssc();

private:
    static constexpr uint8_t MAX_MODULES_AMOUNT{10};
    static inline std::array<Module*, MAX_MODULES_AMOUNT> modules;
    static inline std::span<Module*> active_modules;
    static inline uint8_t modules_amount{0};
};

#endif  // SRC_COMMON_MODULE_HPP_
