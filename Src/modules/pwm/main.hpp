/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_PWM_PWMMODULE_HPP_
#define SRC_MODULES_PWM_PWMMODULE_HPP_

#include <algorithm>
#include "params.hpp"
#include "peripheral/pwm/pwm.hpp"
#include "common/algorithms.hpp"
#include "common/module.hpp"
#include "common/logging.hpp"


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
    uint32_t    engaged_deadline_ms{0};
    uint32_t    last_recv_time_ms{0};
};

class PWMModule : public Module {
public:
    PWMModule() : Module(50, Protocol::CYPHAL_AND_DRONECAN) {}
    void init() override;

    static constexpr uint8_t get_pins_amount() {
        return static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT);
    }

    static int8_t get_pin_channel(uint8_t pin_idx);
    static bool is_pin_enabled(uint8_t pin_idx);
    static uint8_t get_pin_percent(uint8_t pin_idx);

    static std::array<PwmChannelInfo, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> params;
    static inline uint16_t pwm_freq{50};
    static inline uint16_t cmd_ttl{500};
    static inline Logging logger{"PWM"};

protected:
    void update_params() override;
    void spin_once() override;
};

#endif  // SRC_MODULES_PWM_PWMMODULE_HPP_
