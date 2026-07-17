/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Ilia Kliantsevich <iliawork112005@gmail.com>
 */

#include "canopen.hpp"

#include "libcanopen/fdcan.hpp"

REGISTER_MODULE(CanopenModule)

void CanopenModule::init() {
    const libcanopen::TransportApi transport = canopenFdcanGetTransportApi();
    static libcanopen::Node node(transport);
    static libcanopen::LssMaster lss_master(transport);
    _node = &node;
    _lss_master = &lss_master;
    const int16_t result = _node->init(CANOPEN_BITRATE);
    set_health(result >= 0 ? Status::OK : Status::FATAL_MALFANCTION);
    set_mode(Mode::STANDBY);
}

void CanopenModule::spin_once() {
}
