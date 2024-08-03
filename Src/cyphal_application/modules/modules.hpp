/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_MODULES_HPP_
#define SRC_MODULES_HPP_

#include <array>
#include "module.hpp"

std::array<Module*, 4>& get_application_modules();

#endif  // SRC_MODULES_HPP_
