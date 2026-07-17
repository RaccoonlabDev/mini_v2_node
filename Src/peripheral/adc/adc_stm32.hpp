/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_ADC_ADC_STM32_HPP_
#define SRC_PERIPHERAL_ADC_ADC_STM32_HPP_

#include <span>
#include "main.h"

namespace HAL {

// One measured input: which ADC peripheral and which channel on it.
struct AdcChannel {
    ADC_HandleTypeDef* hadc;
    uint32_t channel;  // ADC_CHANNEL_x
};

// Boards that cannot use the default single-ADC1 DMA scan override this in a
// board source file, returning one entry per BoardAdc rank (rank == index).
// Such inputs are read by polling a single conversion on demand, which keeps
// the driver free of DMA/domain constraints — on the STM32H7 in particular,
// ADC3 lives in D3 and could otherwise only be served by BDMA out of SRAM4.
//
// The default returns an empty span: the driver then keeps the legacy
// DMA-scanned ADC1 behaviour driven by Adc::init(channel_count), so existing
// boards are unaffected.
std::span<const AdcChannel> adc_channels();

}  // namespace HAL

#endif  // SRC_PERIPHERAL_ADC_ADC_STM32_HPP_
