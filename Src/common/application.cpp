/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "application.hpp"
#include <array>
#include "periphery/adc/circuit_periphery.hpp"
#include "periphery/led/led.hpp"
#include "params.hpp"
#include "module.hpp"
#include "modules.hpp"
#include "main.h"

#include "periphery/led/led.hpp"
#include "periphery/iwdg/iwdg.hpp"


static int8_t init_board_periphery() {
    LedPeriphery::reset();
    CircuitPeriphery::init();

    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    return 0;
}

__attribute__((noreturn)) void application_entry_point() {
    init_board_periphery();

    auto& modules = get_application_modules();

    for (auto app_module : modules) {
        app_module->init();
    }

    while (true) {
        for (auto app_module : modules) {
            app_module->process();
        }

        auto global_status = Module::get_global_status();
        auto global_mode = Module::get_global_mode();

        LedColor color;
        if (global_status >= Module::Status::MAJOR_FAILURE) {
            color = LedColor::RED_COLOR;
        } else if (global_status >= Module::Status::MINOR_FAILURE) {
            color = LedColor::RED_COLOR;  // should be orange
        } else if (global_mode >= Module::Mode::INITIALIZATION) {
            color = LedColor::RED_COLOR;  // should be blue-red
        } else {
            color = LedColor::BLUE_COLOR;
        }
        LedPeriphery::toggle(color);

        WatchdogPeriphery::refresh();
    }
}
