/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/adc/adc.hpp"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

static uint32_t adc_current_avg = 0;
static uint32_t adc_current_sum = 0;
static uint32_t adc_dma_counter = 0;
static uint16_t adc_dma_buffer[static_cast<uint8_t>(HAL::AdcChannel::ADC_NUMBER_OF_CNANNELS)];

namespace HAL {

int8_t Adc::init() {
    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        return -1;
    }

    auto channels_amount = static_cast<uint8_t>(AdcChannel::ADC_NUMBER_OF_CNANNELS);
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(void*)&adc_dma_buffer, channels_amount) != HAL_OK) {
        return -1;
    }

    _is_adc_already_inited = true;
    return 0;
}

uint16_t Adc::get(AdcChannel channel) {
    if (!_is_adc_already_inited || channel >= AdcChannel::ADC_NUMBER_OF_CNANNELS) {
        return 0;
    }
    if (channel == AdcChannel::ADC_CURRENT) {
        return adc_current_avg;
    }
    return adc_dma_buffer[static_cast<uint8_t>(channel)];
}

}  // namespace HAL

#ifdef HAL_ADC_MODULE_ENABLED
/**
 * @note We assume that hadc->Instance == ADC1 always!
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adc_dma_counter++;

    adc_current_sum += adc_dma_buffer[static_cast<uint8_t>(HAL::AdcChannel::ADC_CURRENT)];
    if (adc_dma_counter > 4000) {
        adc_current_avg = adc_current_sum / adc_dma_counter;
        adc_current_sum = 0;

        adc_dma_counter = 0;
    }

    HAL_ADC_Start_DMA(hadc,
                      (uint32_t*)(void*)&adc_dma_buffer,
                      static_cast<uint8_t>(HAL::AdcChannel::ADC_NUMBER_OF_CNANNELS)
    );
}
#endif
