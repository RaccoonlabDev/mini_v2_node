/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#pragma once

#include <cstdint>

enum class McuFamilyId : std::uint8_t {
    UNKNOWN = 0,
    STM32F103 = 1,
    STM32G0B1 = 2,
    UBUNTU = 3,
    STM32H7 = 4,
};
