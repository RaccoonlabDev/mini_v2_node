/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_BOOTLOADER_HPP_
#define SRC_MODULES_BOOTLOADER_HPP_

#include "module.hpp"
#include "common/logging.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class BootloaderModule : public Module {
public:
    BootloaderModule() : Module(2, Protocol::CYPHAL_AND_DRONECAN) {}
    void init() override;

protected:
    void spin_once() override;

private:
    static inline Logging logger{"BOOT"};
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_BOOTLOADER_HPP_
