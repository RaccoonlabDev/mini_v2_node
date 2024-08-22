/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_ARMING_HPP_
#define SRC_MODULES_ARMING_HPP_

#include "module.hpp"
#include "subscriber.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class ArmingModule : public Module {
public:
    ArmingModule() : Module(0.2) {}
    void init() override;

protected:
    void spin_once() override;

private:
    static void raw_command_cb(const RawCommand_t& msg);
    static inline DronecanSubscriber<RawCommand_t> raw_command_sub;
    static bool is_armed;
    static uint32_t arm_start_time;
    uint32_t prev_eng_time = 0;
    static int64_t disarm_time;
    static constexpr uint8_t RAW_COMMAND_CHANNEL = 0;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_ARMING_HPP_
