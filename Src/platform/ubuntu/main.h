/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MAIN_HPP_
#define SRC_MAIN_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

static inline uint32_t HAL_GetUIDw0() {return 0;}
static inline uint32_t HAL_GetUIDw1() {return 0;}
static inline uint32_t HAL_GetUIDw2() {return 0;}

uint32_t HAL_GetTick();
__attribute__((noreturn)) void platformSpecificRebootForce();
bool platformSpecificRequestRestart();

void platformSpecificReadUniqueID(uint8_t out_uid[4]);
uint32_t platformSpecificGetTimeMs();

#ifdef __cplusplus
}
#endif

#endif  // SRC_MAIN_HPP_
