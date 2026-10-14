/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_NODE_NAME_HPP_
#define SRC_COMMON_NODE_NAME_HPP_

#include <cstring>
#include "params.hpp"
#include "drivers/board_monitor/board_monitor.hpp"

/**
 * @brief Fills the node name parameter before a protocol core reports it, the same way for every
 * protocol:
 * - with NC_DEFAULT_NODE_NAME, if the application defines it, when the name is empty or still the
 *   stock "co.rl.mini";
 * - otherwise with the board name when the name is empty.
 */
inline void apply_default_node_name(ParamIndex_t node_name_param_idx) {
    const auto* name = reinterpret_cast<const char*>(paramsGetStringValue(node_name_param_idx));
    const bool is_empty = name == nullptr || std::strlen(name) == 0;

#ifdef NC_DEFAULT_NODE_NAME
    if (is_empty || std::strcmp(name, "co.rl.mini") == 0) {
        constexpr const char* default_node_name = NC_DEFAULT_NODE_NAME;
        paramsSetStringValue(node_name_param_idx, std::strlen(default_node_name),
                             reinterpret_cast<const uint8_t*>(default_node_name));
    }
#else
    if (is_empty) {
        const auto& [board_name, name_length] = BoardMonitor::get_board_name();
        paramsSetStringValue(node_name_param_idx, name_length,
                             reinterpret_cast<const uint8_t*>(board_name));
    }
#endif
}

#endif  // SRC_COMMON_NODE_NAME_HPP_
