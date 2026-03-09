/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRIVERS_BOARD_MONITOR_BOARD_MONITOR_HPP_
#define SRC_DRIVERS_BOARD_MONITOR_BOARD_MONITOR_HPP_

#include <stdint.h>
#include <limits>
#include <utility>
#include "peripheral/adc/adc.hpp"
#include "adc_mapping.hpp"

#ifdef __cplusplus
extern "C" {
#endif

enum class BoardType : uint8_t {
    POWER_V3_1_0,
    ASPD_V3,
    RANGE_V3_1_0,
    KIRPI_NO_CURRENT,
    KIRPI_WITH_CURRENT,
    MINI_V2_1_1,
    MINI_V3_0_0,

    BOARDS_AMOUNT,
};

class BoardMonitor{
public:
    static int8_t init(){
        return HAL::Adc::init(BoardAdc::DMA_CHANNEL_COUNT);
    }

    /**
     * @return Temperature, Kelvin
     */
    static uint16_t temperature();

    /**
     * @return The current in Amperes if the hardware supports it, otherwise NaN.
     */
    static float current() {
        if (BoardAdc::RANK_CURRENT == BoardAdc::INVALID_RANK) {
            return std::numeric_limits<float>::quiet_NaN();
        }

        if (auto hw_version = hardware_version(); hw_version < 2403 || hw_version > 2450) {
            return std::numeric_limits<float>::quiet_NaN();
        }

        // Current sensor: INA169NA/3K, R = 33K ohm
        // Calibration coefficient was measured experimentally
        constexpr float MAX_SENSOR_CURRENT = 10.0f;
        constexpr float CALIBRATION_COEF = 0.6666667f;
        constexpr float ADC_CURRENT_MULTIPLIER = MAX_SENSOR_CURRENT * CALIBRATION_COEF / 4095.0f;
        uint16_t curr = HAL::Adc::get(BoardAdc::RANK_CURRENT);
        return curr * ADC_CURRENT_MULTIPLIER;
    }

    static float voltage_vin() {
        if (BoardAdc::RANK_VIN == BoardAdc::INVALID_RANK) {
            return std::numeric_limits<float>::quiet_NaN();
        }

        constexpr float ADC_VIN_MULTIPLIER = 1.0f / 64.0f;
        uint16_t volt = HAL::Adc::get(BoardAdc::RANK_VIN);
        return volt * ADC_VIN_MULTIPLIER;
    }

    static float voltage_5v() {
        if (BoardAdc::RANK_5V == BoardAdc::INVALID_RANK) {
            return std::numeric_limits<float>::quiet_NaN();
        }

        constexpr float ADC_5V_MULTIPLIER = 1.0f / 640.0f;
        uint16_t volt = HAL::Adc::get(BoardAdc::RANK_5V);
        return volt * ADC_5V_MULTIPLIER;
    }

    static uint16_t hardware_version() {
        if (BoardAdc::RANK_VERSION == BoardAdc::INVALID_RANK) {
            return 0;
        }
        return HAL::Adc::get(BoardAdc::RANK_VERSION);
    }

    static bool overvoltage() {
        return voltage_5v() > 5.5 || voltage_vin() > 60.0;
    }

    static bool undervoltage() {
        return voltage_5v() < 4.2 || voltage_vin() < 4.5;
    }

    static bool overcurrent() {
        return current() > 60.0;
    }

    static bool overheat() {
        return temperature() > (273.5f + 80.0f);
    }

    static bool is_failure() {
        return overvoltage() || undervoltage() || overcurrent() || overheat();
    }

    static BoardType detect_board_type();
    static std::pair<const char*, uint8_t> get_board_name();
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_DRIVERS_BOARD_MONITOR_BOARD_MONITOR_HPP_
