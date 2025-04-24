/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_TTL_HPP_
#define SRC_COMMON_TTL_HPP_

#include <stdint.h>
#include <limits>
#include "main.h"

/**
 * - SLEEP:     A command has not been received yet or the command timeout has been expired.
 * - STANDBY:   A command received to explicetly do nothing in the default state (UAV is disarmed).
 *              Or the last engaged deadline has been reached (not all UAV report arming states).
 * - ENGAGED:   There is a fresh command to apply a non-default state.
 *              Or there is a default command but the engaged deadline has not been reached yet.
 */
class SetpointTimings {
public:
    SetpointTimings() = default;

    inline void set_cmd_ttl(uint32_t new_cmd_ttl) {
        cmd_ttl = new_cmd_ttl;
    }

    /**
     * @brief Call this if the timeout deadline is reached
     */
    inline void set_sleep_state() {
        last_recv_time_ms = 0;
        engaged_deadline_ms = 0;
    }

    /**
     * @brief Call this when you receive a standby command
     */
    inline void set_standby_state() {
        last_recv_time_ms = HAL_GetTick();
        engaged_deadline_ms = 0;
    }

    /**
     * @brief Call this when you receive a command to apply the default state, but you don't know
     * either it is STANDBY or ENGAGED mode. For example, a motor during the flight can be turned
     * off for a short period while the vehicle is armed.
     */
    inline void set_default_state() {
        last_recv_time_ms = HAL_GetTick();
    }

    /**
     * @brief Call this when you receive an engaged command
     */
    inline void set_engaged_state() {
        last_recv_time_ms = HAL_GetTick();
        engaged_deadline_ms = HAL_GetTick() + cmd_ttl;
    }

    /**
     * @brief Some actuators (for example a gripper) have a persistent state wheather there is a
     * command or not. So, timeouts are basically are not applyed to such devices.
     */
    inline void set_engage_forever() {
        last_recv_time_ms = std::numeric_limits<uint32_t>::max();
        engaged_deadline_ms = std::numeric_limits<uint32_t>::max();
    }

    /**
     * @brief ENGAGED means the actuator is not in the default state (for example a motor is
     * spinning or a servo is rotating) and the timeout deadline has not been reached yet.
     */
    inline bool is_engaged() const {
        return HAL_GetTick() < engaged_deadline_ms;
    }

    /**
     * @brief SLEEP means the command has not been received yet or the timeout has been expired.
     * The actuator goes to the minimal power consumption mode or to the initial state.
     */
    inline bool is_sleeping() const {
        return last_recv_time_ms == 0 && engaged_deadline_ms == 0;
    }

    /**
     * @brief Fresh command state refers either STANDBY or ENGAGED state.
     */
    inline bool is_command_fresh() const {
        return last_recv_time_ms != 0 && HAL_GetTick() < last_recv_time_ms + cmd_ttl;
    }

private:
    uint32_t last_recv_time_ms{0};
    uint32_t engaged_deadline_ms{0};
    uint32_t cmd_ttl{500};
};

#endif  // SRC_COMMON_TTL_HPP_
