/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_ARMING_HPP_
#define SRC_MODULES_ARMING_HPP_

#include "module.hpp"
#include "common/logging.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class ArmingModule : public Module {
public:
    ArmingModule() : Module(2, Protocol::DRONECAN) {}

protected:
    void spin_once() override;

private:
    bool is_armed = {false};
    uint32_t arm_start_time = 0;

    static inline Logging logger{"ARM"};

    // Do not update persistent parameters if the node engaged less than this period of time
    static constexpr uint32_t PERIOD_OF_INSENSITIVITY_MS{3000};
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_ARMING_HPP_
