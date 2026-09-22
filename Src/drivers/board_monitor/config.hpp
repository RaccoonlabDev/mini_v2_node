/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#ifndef SRC_DRIVERS_BOARD_MONITOR_CONFIG_HPP_
#define SRC_DRIVERS_BOARD_MONITOR_CONFIG_HPP_

#include <cstdint>

// Defaults preserve the calibration of boards that do not supply board_monitor_config.hpp.
struct BoardMonitorConfig {
    float vin_volts_per_count{1.0F / 64.0F};
    float v5_volts_per_count{1.0F / 640.0F};
    float current_amps_per_count{10.0F * 0.6666667F / 4095.0F};
    float current_offset_amps{0.0F};
    bool current_requires_version_match{true};
    uint16_t current_version_adc_min{2403};
    uint16_t current_version_adc_max{2450};
    bool current_on_5v{true};
};

#endif  // SRC_DRIVERS_BOARD_MONITOR_CONFIG_HPP_
