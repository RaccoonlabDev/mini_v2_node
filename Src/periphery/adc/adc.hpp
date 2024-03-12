/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_ADC_ADC_HPP_
#define SRC_PERIPHERY_ADC_ADC_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum class AdcChannel : uint8_t {
    ADC_VIN,
    ADC_5V,
    ADC_CURRENT,
    ADC_VERSION,
    ADC_TEMPERATURE,
    ADC_NUMBER_OF_CNANNELS,
};

class AdcPeriphery {
public:
    /**
     * @return 0 on success, otherwise < 0
     */
    static int8_t init();

    /**
     * @return Raw ADC value on success, otherwise 0
     */
    static uint16_t get(AdcChannel channel);

private:
    static inline bool _is_adc_already_inited = false;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_PERIPHERY_ADC_ADC_HPP_
