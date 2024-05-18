/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_PWMMODULE_HPP_
#define SRC_MODULES_PWM_PWMMODULE_HPP_

#include <algorithm>
#include "algorithms.hpp"
#include "dronecan.h"
#include "params.hpp"
#include "logger.hpp"
#include "periphery/pwm/pwm.hpp"


enum class ModuleStatus: uint8_t {
    MODULE_OK        = 0,   // The module is functioning properly
    MODULE_WARN      = 1,   // The module encountered a minor failure
    MODULE_ERROR     = 2,   // The module encountered a major failure
    MODULE_CRITICAL  = 3,   // The module suffered a fatal malfunction
};

enum class CommandType: uint8_t {
    RAW_COMMAND        = 0,
    ARRAY_COMMAND      = 1,
    HARDPOINT_COMMAND  = 2,

    NUMBER_OF_COMMANDS,
};

struct PwmChannelInfo {
    PwmPin pin;
    uint16_t min;
    uint16_t max;
    uint16_t def;
    int16_t channel;
    uint16_t command_val;
    uint32_t cmd_end_time_ms;
    uint32_t next_status_pub_ms;
    uint8_t fb;
};

struct PwmChannelsParamsNames {
    IntegerParamValue_t min;
    IntegerParamValue_t max;
    IntegerParamValue_t def;
    IntegerParamValue_t ch;
    IntegerParamValue_t fb;
};

class PWMModule {
public:
    void spin_once();
    static PWMModule &get_instance();
    static std::array<PwmChannelInfo, static_cast<uint8_t>(PwmPin::PWM_AMOUNT)> params;
    static PwmChannelsParamsNames params_names[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    static ModuleStatus module_status;

protected:
    PWMModule();

private:
    static PWMModule instance;
    void (*callback)(CanardRxTransfer*);
    void (*publish_state)();

    void init();
    void update_params();
    void update_pwm();
    void apply_params();

    static void raw_command_callback(CanardRxTransfer* transfer);
    static void array_command_callback(CanardRxTransfer* transfer);
    static void hardpoint_callback(CanardRxTransfer* transfer);

    static void publish_esc_status();
    static void publish_actuator_status();
    static void publish_raw_command();
    static void publish_array_command();
    static void publish_hardpoint_status();

    static uint16_t pwm_freq;
    static CommandType pwm_cmd_type;

    static uint16_t ttl_cmd;
    uint16_t status_pub_timeout_ms;
    bool verbose;

    static bool publish_error;
    static Logger logger;

    PWMModule& operator = (const PWMModule&) = delete;
    explicit PWMModule(PWMModule *other) = delete;
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
