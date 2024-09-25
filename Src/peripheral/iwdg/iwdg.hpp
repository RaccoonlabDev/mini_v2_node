/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERY_IWDG_HPP_
#define SRC_PERIPHERY_IWDG_HPP_

namespace HAL {

class Watchdog {
public:
    /**
     * @brief Reload IWDG counter with value defined in the reload register
     */
    static void refresh();

    /**
     * @brief Block all refresh calls, so the application will be rebooted soon
     */
    static void request_reboot() {
        reboot_required = true;
    }

private:
    static inline bool reboot_required{false};
};

}  // namespace HAL

#endif  // SRC_PERIPHERY_IWDG_HPP_
