/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/adc/adc.hpp"

namespace HAL {

struct adcState {
    static constexpr uint8_t MAX_CHANNELS = 16;
    uint16_t channels[MAX_CHANNELS];
};
adcState state = {};

int8_t Adc::init(uint8_t channel_count) {
    if (channel_count == 0 || channel_count > adcState::MAX_CHANNELS) {
        return -1;
    }

    _channel_count = channel_count;
    _is_adc_already_inited = true;
    return 0;
}

uint16_t Adc::get(uint8_t rank) {
    if (!_is_adc_already_inited || rank >= _channel_count) {
        return 0;
    }
    return state.channels[rank];
}

int8_t Adc::set(uint8_t rank, uint16_t value) {
    if (!_is_adc_already_inited || rank >= _channel_count || value > 4095) {
        return -1;
    }

    state.channels[rank] = value;
    return 0;
}

}  // namespace HAL
