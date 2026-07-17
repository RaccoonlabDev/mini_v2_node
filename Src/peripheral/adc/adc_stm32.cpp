/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "peripheral/adc/adc.hpp"
#include "peripheral/adc/adc_stm32.hpp"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

static constexpr uint8_t ADC_MAX_DMA_CHANNELS = 16;
static uint16_t adc_dma_buffer[ADC_MAX_DMA_CHANNELS];

// Long enough for the internal temperature sensor, which needs a far greater
// acquisition time than a resistor divider (~9us minimum on the STM32H7).
#if defined(STM32H753xx)
static constexpr uint32_t ADC_POLL_SAMPLETIME = ADC_SAMPLETIME_387CYCLES_5;
#elif defined(STM32G0B1xx)
static constexpr uint32_t ADC_POLL_SAMPLETIME = ADC_SAMPLETIME_79CYCLES_5;
#else
static constexpr uint32_t ADC_POLL_SAMPLETIME = ADC_SAMPLETIME_55CYCLES_5;
#endif

static constexpr uint32_t ADC_POLL_TIMEOUT_MS = 10;

namespace HAL {

// Weak default: no board-provided channels -> legacy DMA-scanned ADC1.
// A board enables polled reads (and extra ADCs) by defining HAL::adc_channels().
[[gnu::weak]] std::span<const AdcChannel> adc_channels() {
    return {};
}

namespace {

int8_t calibrate(ADC_HandleTypeDef* hadc) {
#ifdef STM32H753xx
    if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
#else
    if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) {
#endif
        return -1;
    }
    return 0;
}

// Single blocking conversion. Cheap enough for the slow rates the monitors use.
uint16_t read_channel(const AdcChannel& input) {
    ADC_ChannelConfTypeDef config{};
    config.Channel = input.channel;
    config.Rank = ADC_REGULAR_RANK_1;
    config.SamplingTime = ADC_POLL_SAMPLETIME;
#ifdef STM32H753xx
    config.SingleDiff = ADC_SINGLE_ENDED;
    config.OffsetNumber = ADC_OFFSET_NONE;
    config.Offset = 0;
#endif

    if (HAL_ADC_ConfigChannel(input.hadc, &config) != HAL_OK) {
        return 0;
    }

    if (HAL_ADC_Start(input.hadc) != HAL_OK) {
        return 0;
    }

    uint16_t value = 0;
    if (HAL_ADC_PollForConversion(input.hadc, ADC_POLL_TIMEOUT_MS) == HAL_OK) {
        value = static_cast<uint16_t>(HAL_ADC_GetValue(input.hadc));
    }

    HAL_ADC_Stop(input.hadc);
    return value;
}

}  // namespace

int8_t Adc::init(uint8_t channel_count) {
    if (channel_count == 0 || channel_count > ADC_MAX_DMA_CHANNELS) {
        return -1;
    }

    const auto channels = adc_channels();

    if (!channels.empty()) {
        // Calibrate every distinct ADC referenced by the board's channel table.
        for (size_t idx = 0; idx < channels.size(); idx++) {
            auto* hadc = channels[idx].hadc;
            if (hadc == nullptr) {
                return -1;
            }

            bool already_done = false;
            for (size_t prev = 0; prev < idx; prev++) {
                if (channels[prev].hadc == hadc) {
                    already_done = true;
                    break;
                }
            }

            if (!already_done && calibrate(hadc) != 0) {
                return -1;
            }
        }

        _channel_count = static_cast<uint8_t>(channels.size());
        _is_adc_already_inited = true;
        return 0;
    }

    if (calibrate(&hadc1) != 0) {
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

    const auto channels = adc_channels();
    if (!channels.empty()) {
        return read_channel(channels[rank]);
    }

    return adc_dma_buffer[rank];
}

}  // namespace HAL

#ifdef HAL_ADC_MODULE_ENABLED
/**
 * @note Only used by the legacy DMA-scanned ADC1 path; polled boards never start a
 *       DMA transfer, so this stays a no-op for them.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (!HAL::adc_channels().empty()) {
        return;
    }

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
