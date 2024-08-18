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
#include "main.h"

#include "periphery/led/led.hpp"
#include "periphery/iwdg/iwdg.hpp"

static int8_t init_board_periphery() {
    Board::Led::reset();
    CircuitPeriphery::init();

    auto libparams_integers_amount = (ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
    auto libparams_strings_amount = NUM_OF_STR_PARAMS;
    paramsInit(libparams_integers_amount, libparams_strings_amount, -1, 1);
    paramsLoad();

    return 0;
}

/**
 * @brief Since v2 hardware version all boards have an internal RGB LED and comply with the
 * RaccoonLab LED indication standard.
 * The LED meaning is as close to the PX4 LED Meanings and Ardupilot LED meanings as possible and
 * is dedicated to help you to understand a board state or a possible problem.
 *
 * General-purpose states:
 * 1. [Blinking Blue/Red]   Initialization
 * 2. [Blinking Red/Blue]   Maintenance
 * 3. [Blinking Blue]       Good
 * 4. [Blinking yellow]     Minor failure
 * 5. [Blinking magenta]    Major failure
 * 6. [Blinking red]        Fatal malfanction
 * @note https://docs.raccoonlab.co/guide/intro/leds.html
 */
static void blink_board_led() {
    auto status = ModuleManager::get_global_status();
    auto mode = ModuleManager::get_global_mode();

    std::pair<Board::Led::Color, Board::Led::Color> colors;
    if (mode == Module::Mode::INITIALIZATION) {
        colors = {Board::Led::Color::RED, Board::Led::Color::BLUE};
    } else if (mode == Module::Mode::MAINTENANCE) {
        colors = {Board::Led::Color::RED, Board::Led::Color::BLUE};
    } else if (status >= Module::Status::FATAL_MALFANCTION) {
        colors = {Board::Led::Color::RED, Board::Led::Color::BLACK};
    } else if (status >= Module::Status::MAJOR_FAILURE) {
        colors = {Board::Led::Color::MAGENTA, Board::Led::Color::BLACK};
    } else if (status >= Module::Status::MINOR_FAILURE) {
        colors = {Board::Led::Color::YELLOW, Board::Led::Color::BLACK};
    } else {
        colors = {Board::Led::Color::BLUE, Board::Led::Color::BLACK};
    }

    Board::Led::blink(colors.first, colors.second);
}

Module::Protocol get_protocol() {
    auto system_protocol = paramsGetIntegerValue(PARAM_SYSTEM_PROTOCOL);

    Module::Protocol protocol;
    if (system_protocol == static_cast<int32_t>(Module::Protocol::CYPHAL)) {
        protocol = Module::Protocol::CYPHAL;
    } else {
        protocol = Module::Protocol::DRONECAN;
    }

    return protocol;
}

__attribute__((noreturn)) void application_entry_point() {
    init_board_periphery();
    ModuleManager::init(get_protocol());

    while (true) {
        ModuleManager::process();
        blink_board_led();
        WatchdogPeriphery::refresh();
    }
}
