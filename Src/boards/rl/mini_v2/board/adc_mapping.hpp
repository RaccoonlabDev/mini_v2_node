/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_BOARDS_RL_MINI_V2_ADC_MAPPING_HPP_
#define SRC_BOARDS_RL_MINI_V2_ADC_MAPPING_HPP_

#include <stdint.h>

namespace BoardAdc {

static constexpr uint8_t INVALID_RANK = 0xFF;
static constexpr uint8_t DMA_CHANNEL_COUNT = 6;

static constexpr uint8_t RANK_VIN = 0;
static constexpr uint8_t RANK_5V = 1;
static constexpr uint8_t RANK_CURRENT = 2;
static constexpr uint8_t RANK_VERSION = 3;
static constexpr uint8_t RANK_TEMPERATURE = 4;

}  // namespace BoardAdc

#endif  // SRC_BOARDS_RL_MINI_V2_ADC_MAPPING_HPP_
