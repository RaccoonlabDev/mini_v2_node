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
#include "peripheral/led/led.hpp"
#include "peripheral/iwdg/iwdg.hpp"
#ifdef STM32G0B1xx
#include "adc.h"
#endif

static const std::array<std::pair<uint16_t, uint16_t>, (int)BoardType::BOARDS_AMOUNT> hw_info = {{
    {59, 69},
    {436, 460},
    {623, 653},
    {2290, 2339},
    {2403, 2450},
    {3839, 3856},
    {3880, 3896},
}};

static const std::array<std::pair<const char*, uint16_t>, (int)BoardType::BOARDS_AMOUNT> names = {{
    {"co.rl.power.v3.1.0", 18},
    {"co.rl.aspd.v3", 14},
    {"co.rl.range.v3.1.0", 18},
    {"arl.kirpi.v2.2.0", 17},
    {"arl.kirpi.v2.3.0", 17},
    {"co.rl.mini.v2", 14},
    {"co.rl.mini.v3", 14},
}};

uint16_t CircuitPeriphery::temperature() {
    auto adc_12b = AdcPeriphery::get(AdcChannel::ADC_TEMPERATURE);
    uint16_t temperature_kelvin;
#ifdef STM32G0B1xx
    temperature_kelvin = __HAL_ADC_CALC_TEMPERATURE(3300, adc_12b, ADC_RESOLUTION_12B) + 273;
#else  // STM32F103xB
    static const uint16_t TEMP_REF = 25;
    static const uint16_t ADC_REF = 1750;   ///< v_ref / 3.3 * 4095
    static const uint16_t AVG_SLOPE = 5;    ///< avg_slope/(3.3/4096)
    temperature_kelvin = (ADC_REF - adc_12b) / AVG_SLOPE + TEMP_REF + 273;
#endif
    return temperature_kelvin;
}

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
