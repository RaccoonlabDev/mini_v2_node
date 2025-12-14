/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_CYPHAL_CYPHAL_MODULE_HPP_
#define SRC_CYPHAL_APPLICATION_CYPHAL_CYPHAL_MODULE_HPP_

#include "module.hpp"
#include "libcpnode/cyphal.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class CyphalModule : public Module {
public:
    CyphalModule();

    void init() override;

protected:
    void spin_once() override;

private:
    libcpnode::Cyphal cyphal;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_CYPHAL_CYPHAL_MODULE_HPP_
