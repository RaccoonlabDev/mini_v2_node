/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#ifndef SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_
#define SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_

#include "libcanopen/canopen.hpp"
#include "module.hpp"

class CanopenModule : public Module {
public:
    CanopenModule() : Module(10.0F, Protocol::DRONECAN) {}
    void init() override;

    static libcanopen::Node* getNode() {
        return _node;
    }

protected:
    void spin_once() override;

private:
    static constexpr uint32_t CANOPEN_BITRATE = 250000U;
    static inline libcanopen::Node* _node{nullptr};
};

#endif  // SRC_MODULES_DRONECAN_CANOPEN_CANOPEN_HPP_
