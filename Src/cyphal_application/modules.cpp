/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "modules/cyphal_module/cyphal_module.hpp"
#include "modules/setpoint/setpoint.hpp"
#include "modules/feedback/feedback.hpp"
#include "modules/circuit_status/circuit_status.hpp"

static CyphalModule cyphal_module;
static SetpointModule setpoint;
static FeedbackModule feedback;
static CircuitStatus crct;
static std::array<Module*, 4> modules = {{ &cyphal_module, &setpoint, &feedback, &crct }};

std::array<Module*, 4>& get_application_modules() {
    return modules;
}
