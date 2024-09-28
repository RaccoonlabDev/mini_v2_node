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
#include "common/logging.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "common/module.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"


enum class CommandType: uint8_t {
    RAW_COMMAND,
    ARRAY_COMMAND,
    HARDPOINT_COMMAND,

    NUMBER_OF_COMMANDS,
};

struct PwmChannelsParamsNames {
    uint8_t min;
    uint8_t max;
    uint8_t def;
    uint8_t ch;
};

struct PwmChannelInfo {
    const PwmChannelsParamsNames names;
    HAL::PwmPin pin;
    uint16_t    min{0};
    uint16_t    max{0};
    uint16_t    def{0};
    int16_t     channel{-1};
    uint16_t    command_val{0};
    uint32_t    cmd_end_time_ms{0};
    bool        is_engaged{false};
};

class PWMModule : public Module {
public:
    void init() override;

    inline PWMModule() : Module(50, Protocol::DRONECAN) {}

    static std::array<PwmChannelInfo, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> params;

protected:
    void update_params() override;
    void spin_once() override;


private:
    void (*callback)(CanardRxTransfer*) = {};
    void (*publish_state)() = {};

    void update_pwm();
    void apply_params();

    static void raw_command_cb(const RawCommand_t& msg);
    static void array_command_callback(CanardRxTransfer* transfer);
    static void hardpoint_callback(CanardRxTransfer* transfer);
    static void arming_status_callback(const ArmingStatus& msg);

    static inline DronecanSubscriber<RawCommand_t> raw_command_sub;

    static uint16_t pwm_freq;
    static CommandType pwm_cmd_type;

    static uint16_t ttl_cmd;
    uint16_t status_pub_timeout_ms = 0;
    bool verbose = false;

    static bool publish_error;
    static inline Logging logger{"PWM"};
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
