/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRONECAN_APPLICATION_DRONECAN_DRONECAN_MODULE_HPP_
#define SRC_DRONECAN_APPLICATION_DRONECAN_DRONECAN_MODULE_HPP_

#include "module.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class DronecanModule : public Module {
public:
    DronecanModule();

    void init() override;

protected:
    void spin_once() override;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_DRONECAN_APPLICATION_DRONECAN_DRONECAN_MODULE_HPP_
