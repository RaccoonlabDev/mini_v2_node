#ifndef SRC_MODULE_PWMMODULE_HPP_
#define SRC_MODULE_PWMMODULE_HPP_

#include "dronecan.h"
#include "params.hpp"

#include "../logger.hpp"
#include "periphery/pwm/pwm.hpp"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
// #include "uavcan/equipment/hardpoint/Command.h"

enum class ModuleStatus: uint8_t {
    MODULE_OK        = 0,   // The module is functioning properly
    MODULE_WARN      = 1,   // The module encountered a minor failure
    MODULE_ERROR     = 2,   // The module encountered a major failure
    MODULE_CRITICAL  = 3,   // The module suffered a fatal malfunction
};

struct PwmChannelInfo {
    PwmPin pin;
    uint32_t min;
    uint32_t max;
    uint32_t def;
    uint8_t channel;
    uint16_t command_val;
    uint32_t cmd_end_time_ms;
};

struct PwmChannelsParamsNames {
    IntegerParamValue_t min;
    IntegerParamValue_t max;
    IntegerParamValue_t def;
    IntegerParamValue_t ch;
};

class PWMModule {
public:

    static PWMModule &get_instance();
    static PwmChannelInfo params[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    static PwmChannelsParamsNames params_names[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    void init();
    void spin_once();
    void update_params();
    void update_pwm();
    void apply_params();

protected:
    PWMModule();

private:
    static PWMModule instance;
    bool verbose;
    void (*callback)(CanardRxTransfer*);
    void (*publish_state)();

    // static void lights_command_callback(CanardRxTransfer* transfer);
    static void raw_command_callback(CanardRxTransfer* transfer);
    static void array_command_callback(CanardRxTransfer* transfer);
    // static void hardpoint_command_callback(CanardRxTransfer* transfer);

    static void publish_raw_command();
    static void publish_array_command();
    // static void publish_hardpoint_command(PwmPin pwm_pin);
    
    static ModuleStatus module_status;

    static uint32_t pwm_freq;
    static uint8_t pwm_cmd_type;

    static uint16_t ttl_cmd;
    static uint8_t node_id;
    static bool publish_error;
    static Logger logger;

    PWMModule& operator = (const PWMModule&) = delete;
    PWMModule(PWMModule &other) = delete;
};

#endif //SRC_MODULE_PWMMODULE_HPP_
