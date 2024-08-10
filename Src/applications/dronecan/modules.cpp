/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules.hpp"
#include "modules/dronecan/core/dronecan_module.hpp"
#include "modules/dronecan/pwm/PWMModule.hpp"
#include "modules/dronecan/circuit_status/CircuitStatusModule.hpp"

static DronecanModule dronecan_module;
static CircuitStatus circuit_status;
static PWMModule pwm_module;
static std::array<Module*, 3> modules = { &dronecan_module, &circuit_status, &pwm_module };

std::array<Module*, 3>& get_application_modules() {
    return modules;
}
