/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova  <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "board_status.hpp"
#include "drivers/board_monitor/board_monitor.hpp"

REGISTER_MODULE(BoardStatusModule)

static topics::CurrentRail current_rail() {
    if (BoardAdc::RANK_CURRENT == BoardAdc::INVALID_RANK) {
        return topics::CurrentRail::NONE;
    }
    return BOARD_MONITOR_CONFIG.current_on_5v ? topics::CurrentRail::V5 : topics::CurrentRail::VIN;
}

// A NaN fails every comparison, so a quantity the board can't measure never raises a flag.
static uint8_t error_flags(const topics::BoardStatus& status) {
    uint8_t flags = 0;
    if (status.voltage_5v < BoardMonitor::V5_MIN_V) {
        flags |= topics::BOARD_ERROR_V5_UNDERVOLTAGE;
    }
    if (status.voltage_5v > BoardMonitor::V5_MAX_V) {
        flags |= topics::BOARD_ERROR_V5_OVERVOLTAGE;
    }
    if (status.voltage_vin < BoardMonitor::VIN_MIN_V) {
        flags |= topics::BOARD_ERROR_VIN_UNDERVOLTAGE;
    }
    if (status.voltage_vin > BoardMonitor::VIN_MAX_V) {
        flags |= topics::BOARD_ERROR_VIN_OVERVOLTAGE;
    }
    if (status.current > BoardMonitor::CURRENT_MAX_A) {
        flags |= topics::BOARD_ERROR_OVERCURRENT;
    }
    if (status.temperature > BoardMonitor::TEMPERATURE_MAX_K) {
        flags |= topics::BOARD_ERROR_OVERHEAT;
    }
    return flags;
}

void BoardStatusModule::spin_once() {
    topics::BoardStatus status{
        .temperature = static_cast<float>(BoardMonitor::temperature()),
        .voltage_5v = BoardMonitor::voltage_5v(),
        .voltage_vin = BoardMonitor::voltage_vin(),
        .current = BoardMonitor::current(),
        .current_rail = current_rail(),
        .hardware_version_adc = BoardMonitor::hardware_version(),
        .error_flags = 0,
    };
    status.error_flags = error_flags(status);
    status_pub.publish(status);
}
