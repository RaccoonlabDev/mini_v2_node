/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/adc/adc.hpp"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

static constexpr uint8_t ADC_MAX_DMA_CHANNELS = 16;
static uint16_t adc_dma_buffer[ADC_MAX_DMA_CHANNELS];

namespace HAL {

int8_t Adc::init(uint8_t channel_count) {
    if (channel_count == 0 || channel_count > ADC_MAX_DMA_CHANNELS) {
        return -1;
    }

    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        return -1;
    }

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(void*)&adc_dma_buffer, channel_count) != HAL_OK) {
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
    return adc_dma_buffer[rank];
}

}  // namespace HAL

#ifdef HAL_ADC_MODULE_ENABLED
/**
 * @note We assume that hadc->Instance == ADC1 always!
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    auto channel_count = HAL::Adc::channel_count();
    if (channel_count == 0) {
        return;
    }

    HAL_ADC_Start_DMA(hadc,
                      (uint32_t*)(void*)&adc_dma_buffer,
                      channel_count
    );
}
#endif
