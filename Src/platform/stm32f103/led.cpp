/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "periphery/led/led.hpp"
#include "main.h"

static void write_red(GPIO_PinState state) {
    HAL_GPIO_WritePin(INT_RGB_LED_RED_GPIO_Port, INT_RGB_LED_RED_Pin, state);
}
static void write_green(GPIO_PinState state) {
    HAL_GPIO_WritePin(INT_RGB_LED_GREEN_GPIO_Port, INT_RGB_LED_GREEN_Pin, state);
}
static void write_blue(GPIO_PinState state) {
    HAL_GPIO_WritePin(INT_RGB_LED_BLUE_GPIO_Port, INT_RGB_LED_BLUE_Pin, state);
}

void LedPeriphery::reset() {
    write_red(GPIO_PIN_SET);
    write_green(GPIO_PIN_SET);
    write_blue(GPIO_PIN_SET);
}

void LedPeriphery::toggle(LedColor color) {
    auto crnt_time_ms = HAL_GetTick();
    GPIO_PinState state = (crnt_time_ms % 1000 > 500) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    write_red(color == LedColor::RED_COLOR ? state : GPIO_PIN_SET);
    write_green(color == LedColor::BLUE_COLOR ? state : GPIO_PIN_SET);
    write_blue(color == LedColor::GREEN_COLOR ? state : GPIO_PIN_SET);
}
