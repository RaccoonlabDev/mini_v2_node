/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_DRIVERS_RCPWM_RCPWM_HPP_
#define SRC_DRIVERS_RCPWM_RCPWM_HPP_

#include <cstdint>
#include <array>
#include "peripheral/pwm/pwm.hpp"

namespace Driver {

struct RcoutChannel {
    struct RcoutParamsIndexes {
        uint8_t min;
        uint8_t max;
        uint8_t def;
        uint8_t ch;
    };
    const RcoutParamsIndexes idx;

    HAL::PwmPin pin;
    uint16_t    ch{0};
    uint16_t    min{0};
    uint16_t    max{0};
    uint16_t    def{0};
    int16_t     channel{-1};

    RcoutChannel(uint8_t min_, uint8_t max_, uint8_t def_, uint8_t ch_, HAL::PwmPin pin_)
        : idx{RcoutParamsIndexes{min_, max_, def_, ch_}},
          pin{pin_},
          ch{ch_},
          min{min_},
          max{max_},
          def{def_}
    {
    }

    /**
     * @brief The default PWM duration withing [1000; 2000] us
     */
    void set_default() const;

    /**
     * @brief Set output as percentage [0 .. 100], -1 to set default value
     */
    virtual void set_percent(int8_t percent) const = 0;
};

struct RcpwmChannel: public RcoutChannel {
    RcpwmChannel(uint8_t min_, uint8_t max_, uint8_t def_, uint8_t ch_, HAL::PwmPin pin_)
        : RcoutChannel(min_, max_, def_, ch_, pin_) {}

    /**
     * @brief Set output as percentage [0 .. 100], -1 to set default value
     */
    void set_percent(int8_t percent) const override;

    /**
     * @brief Set output value as a normalized signed float [-1.0 .. +1.0]
     * where -1.0 means min, +1.0 means max and 0.0 means the neutral position
     */
    void set_normalized_signed(float value) const;

    /**
     * @brief Set output value as a normalized unsigned float [0.0 .. 1.0]
     * where 0.0 means min and +1.0 means max output
     */
    void set_normalized_unsigned(float value) const;

    /**
     * @brief Set output as int14 command [-8192, 8191]
     */
    void set_int14(uint16_t cmd_int14) const;

    /**
     * @brief Set output as us [1000 .. 2000]
     */
    void _set_us(uint16_t duration_us) const;
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
