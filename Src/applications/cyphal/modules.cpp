/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/cyphal/core/cyphal_module.hpp"
#include "modules/cyphal/setpoint/setpoint.hpp"
#include "modules/cyphal/feedback/feedback.hpp"
#include "modules/cyphal/circuit_status/circuit_status.hpp"

static CyphalModule cyphal_module;
static SetpointModule setpoint;
static FeedbackModule feedback;
static CircuitStatus crct;
static std::array<Module*, 4> modules = {{ &cyphal_module, &setpoint, &feedback, &crct }};

std::array<Module*, 4>& get_application_modules() {
    return modules;
}
