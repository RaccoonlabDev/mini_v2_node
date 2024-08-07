/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_MODULE_HPP_
#define SRC_COMMON_MODULE_HPP_

#include <cstdint>
#include "main.h"


class Module {
public:
    enum class Status: uint8_t {
        OK                  = 0,    // ok          nominal
        MINOR_FAILURE       = 1,    // warning     advisory
        MAJOR_FAILURE       = 2,    // error       caution
        FATAL_MALFANCTION   = 3,    // critical    warning
    };

    enum class Mode: uint8_t {
        OPEARTIONAL         = 0,    // after successful initialization
        INITIALIZATION      = 1,    // after startup
        MAINTENANCE         = 2,    // calibration, self-test
    };

    explicit Module(float frequency);

    virtual void init();

    Status get_health() const;

    Mode get_mode() const;

    void process();

    static Status get_global_status();
    static Mode get_global_mode();
    static uint8_t get_vssc();

protected:
    virtual void update_params() {};
    virtual void spin_once() = 0;

    static uint32_t period_ms_from_frequency(float frequency);

    Status health{Status::OK};
    Mode mode{Mode::INITIALIZATION};

    uint32_t period_ms;
    uint32_t next_spin_time_ms{0};
};

#endif  // SRC_COMMON_MODULE_HPP_
