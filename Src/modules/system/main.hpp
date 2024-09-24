/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_SYSTEM_HPP_
#define SRC_MODULES_SYSTEM_HPP_

#include "module.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class SystemModule : public Module {
public:
    SystemModule() : Module(2, Protocol::DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;
    bool need_notification{false};
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_SYSTEM_HPP_
