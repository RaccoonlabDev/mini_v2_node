/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_ADC_ADC_HPP_
#define SRC_PERIPHERAL_ADC_ADC_HPP_

#include <stdint.h>

namespace HAL {

class Adc {
public:
    // Zero opts into the HAL sequence length on F1/H7 or the board polling table length.
    // Nonzero counts retain the legacy DMA length; a polling table defines its own length.
    static int8_t init(uint8_t channel_count = 0);
    static uint16_t get(uint8_t rank);
    static uint8_t channel_count() { return _channel_count; }
#ifdef USE_PLATFORM_UBUNTU
    static int8_t set(uint8_t rank, uint16_t value);
#endif
private:
    static inline bool _is_adc_already_inited = false;
    static inline uint8_t _channel_count = 0;
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_ADC_ADC_HPP_
