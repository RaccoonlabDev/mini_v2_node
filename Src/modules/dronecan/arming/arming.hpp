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
    ArmingModule() : Module(2, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;

private:
    Mode global_mode;
    bool is_armed = {false};
    uint32_t arm_start_time = 0;
    uint32_t prev_eng_time = 0;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_ARMING_HPP_
