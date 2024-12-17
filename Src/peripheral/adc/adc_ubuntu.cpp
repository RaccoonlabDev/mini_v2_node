/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/adc/adc.hpp"

namespace HAL {

struct adcState {
    uint16_t current;
    uint16_t v_in;
    uint16_t v_5v;
    uint16_t version;
    uint16_t temperature;
};
adcState state = {};

int8_t Adc::init() {
    state.temperature = 3115;
    state.current = 2048;
    state.v_in = 640;
    state.v_5v = 3200;
    _is_adc_already_inited = true;
    return 0;
}

uint16_t Adc::get(AdcChannel channel) {
    switch (channel)
    {
    case AdcChannel::ADC_VIN:
        return state.v_in;
    case AdcChannel::ADC_CURRENT:
        return state.current;
    case AdcChannel::ADC_TEMPERATURE:
        return state.temperature;
    case AdcChannel::ADC_5V:
        return state.v_5v;
    default:
        break;
    }
    return 0;
}

}  // namespace HAL
