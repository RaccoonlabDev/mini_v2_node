/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_IWDG_HPP_
#define SRC_PERIPHERY_IWDG_HPP_

class WatchdogPeriphery {
public:
    static void refresh();
};

#endif  // SRC_PERIPHERY_IWDG_HPP_
