/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/iwdg/iwdg.hpp"
#include "main.h"

#ifdef HAL_IWDG_MODULE_ENABLED
extern IWDG_HandleTypeDef hiwdg;
#endif  // HAL_IWDG_MODULE_ENABLED

void WatchdogPeriphery::refresh() {
#ifdef HAL_IWDG_MODULE_ENABLED
    HAL_IWDG_Refresh(&hiwdg);
#endif  // HAL_IWDG_MODULE_ENABLED
}
