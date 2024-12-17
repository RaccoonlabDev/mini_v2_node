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

enum class AdcChannel : uint8_t {
    ADC_VIN,
    ADC_5V,
    ADC_CURRENT,
    ADC_VERSION,
    ADC_TEMPERATURE,
    ADC_NUMBER_OF_CNANNELS,
};

class Adc {
public:
    static int8_t init();
    static uint16_t get(AdcChannel channel);
private:
    static inline bool _is_adc_already_inited = false;
};

}  // namespace HAL

#endif  // SRC_PERIPHERAL_ADC_ADC_HPP_
