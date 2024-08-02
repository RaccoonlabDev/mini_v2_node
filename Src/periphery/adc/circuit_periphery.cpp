/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "circuit_periphery.hpp"
#include <array>
#include "application.hpp"
#include "main.h"
#include "params.hpp"
#include "periphery/led/led.hpp"
#include "periphery/iwdg/iwdg.hpp"

static const std::array<std::pair<uint16_t, uint16_t>, (int)BoardType::BOARDS_AMOUNT> hw_info = {{
    {436, 460},
    {2290, 2339},
    {2403, 2450},
    {3839, 3856},
    {3880, 3896},
}};

static const std::array<std::pair<const char*, uint16_t>, (int)BoardType::BOARDS_AMOUNT> names = {{
    {"co.rl.aspd.v3", 14},
    {"arl.kirpi.v2.2.0", 17},
    {"arl.kirpi.v2.3.0", 17},
    {"co.rl.mini.v2", 14},
    {"co.rl.mini.v3", 14},
}};

BoardType CircuitPeriphery::detect_board_type() {
    auto hardware_version = CircuitPeriphery::hardware_version();

    int board_counter = 0;

    for (auto [min_adc, max_adc] : hw_info) {
        if (hardware_version > min_adc && hardware_version < max_adc) {
            break;
        }
        board_counter++;
    }

    auto detected_board = static_cast<BoardType>(board_counter);
    return (detected_board < BoardType::BOARDS_AMOUNT) ? detected_board : BoardType::MINI_V2_1_1;
}

std::pair<const char*, uint8_t> CircuitPeriphery::get_board_name() {
    return names[(uint8_t)detect_board_type()];
}
