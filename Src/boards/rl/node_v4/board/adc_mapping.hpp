/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_BOARDS_RL_NODE_V4_ADC_MAPPING_HPP_
#define SRC_BOARDS_RL_NODE_V4_ADC_MAPPING_HPP_

#include <stdint.h>

namespace BoardAdc {

static constexpr uint8_t INVALID_RANK = 0xFF;
static constexpr uint8_t DMA_CHANNEL_COUNT = 1;

static constexpr uint8_t RANK_VIN = INVALID_RANK;
static constexpr uint8_t RANK_5V = INVALID_RANK;
static constexpr uint8_t RANK_CURRENT = INVALID_RANK;
static constexpr uint8_t RANK_VERSION = INVALID_RANK;
static constexpr uint8_t RANK_TEMPERATURE = INVALID_RANK;

}  // namespace BoardAdc

#endif  // SRC_BOARDS_RL_NODE_V4_ADC_MAPPING_HPP_
