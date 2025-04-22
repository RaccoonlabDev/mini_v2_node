/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_DRIVERS_RCPWM_RCPWM_HPP_
#define SRC_DRIVERS_RCPWM_RCPWM_HPP_

#include <cstdint>
#include "peripheral/pwm/pwm.hpp"
#include <array>

namespace Driver {

struct RcpwmChannel {
    struct RcpwmChannelParamsIndexes {
        uint8_t min;
        uint8_t max;
        uint8_t def;
        uint8_t ch;
    };

    const RcpwmChannelParamsIndexes names;
    HAL::PwmPin pin;
    uint16_t    min{0};
    uint16_t    max{0};
    uint16_t    def{0};
    int16_t     channel{-1};

    /**
     * @brief Set output as us [1000 .. 2000]
     */
    void set_us(uint16_t duration_us);

    /**
     * @brief Set output as percentage [0 .. 100]
     */
    void set_percent(uint8_t percent);

    /**
     * @brief Set output value as a normalized servo float [-1.0 .. +1.0]
     */
    void set_normalized_servo(float value);

    /**
     * @brief Set output value as a normalized motor float [0.0 .. 1.0]
     */
    void set_normalized_motor(float value);

    /**
     * @brief Set output as int14 command [-8192, 8191]
     */
    void set_int14(uint16_t cmd_int14);

    /**
     * @brief The default PWM duration withing [1000; 2000] us
     */
    void set_default();
};

class RCPWM {
public:
    /**
     * @brief Initialize all PWM pins
     * @return 0 on success, otherwise negative error code
     */
    static int8_t init();

    /**
     * @brief
     */
    static void update_params();

    /**
     * @brief Set frequency withing range [50, 400] Hz
     */
    static void set_frequency(uint16_t frequency);

    /**
     * @return RCPWM percent in range [0, 100] if a given pin index exists, otwerwise return 0
     */
    static uint8_t get_pin_percent(uint8_t pin_idx);

    static constexpr uint8_t get_pins_amount() {
        return static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT);
    }

    static int8_t get_pin_channel(uint8_t pin_idx);
    static bool is_pin_enabled(uint8_t pin_idx);

// private:
    static std::array<RcpwmChannel, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> channels;
};

}  // namespace Driver

#endif  // SRC_DRIVERS_RCPWM_RCPWM_HPP_
