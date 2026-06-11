/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "led.hpp"
#include <cstddef>
#include "main.h"

namespace Board {

#if defined(BOARD_LED_ACTIVE_HIGH) && BOARD_LED_ACTIVE_HIGH
static constexpr GPIO_PinState LED_ON = GPIO_PIN_SET;
static constexpr GPIO_PinState LED_OFF = GPIO_PIN_RESET;
#else
static constexpr GPIO_PinState LED_ON = GPIO_PIN_RESET;
static constexpr GPIO_PinState LED_OFF = GPIO_PIN_SET;
#endif

#ifndef INTERNAL_LED_RED_GPIO_Port
    #ifdef INT_RGB_LED_RED_GPIO_Port
        #define INTERNAL_LED_RED_GPIO_Port      INT_RGB_LED_RED_GPIO_Port
        #define INTERNAL_LED_RED_Pin            INT_RGB_LED_RED_Pin
    #elif defined(NLED_RED_GPIO_Port)
        #define INTERNAL_LED_RED_GPIO_Port      NLED_RED_GPIO_Port
        #define INTERNAL_LED_RED_Pin            NLED_RED_Pin
    #endif
#endif
#ifndef INTERNAL_LED_GREEN_GPIO_Port
    #ifdef INT_RGB_LED_GREEN_GPIO_Port
        #define INTERNAL_LED_GREEN_GPIO_Port    INT_RGB_LED_GREEN_GPIO_Port
        #define INTERNAL_LED_GREEN_Pin          INT_RGB_LED_GREEN_Pin
    #elif defined(NLED_GREEN_GPIO_Port)
        #define INTERNAL_LED_GREEN_GPIO_Port    NLED_GREEN_GPIO_Port
        #define INTERNAL_LED_GREEN_Pin          NLED_GREEN_Pin
    #endif
#endif
#ifndef INTERNAL_LED_BLUE_GPIO_Port
    #ifdef INT_RGB_LED_BLUE_GPIO_Port
        #define INTERNAL_LED_BLUE_GPIO_Port     INT_RGB_LED_BLUE_GPIO_Port
        #define INTERNAL_LED_BLUE_Pin           INT_RGB_LED_BLUE_Pin
    #elif defined(NLED_BLUE_GPIO_Port)
        #define INTERNAL_LED_BLUE_GPIO_Port     NLED_BLUE_GPIO_Port
        #define INTERNAL_LED_BLUE_Pin           NLED_BLUE_Pin
    #endif
#endif

static void write_red(bool enabled) {
    GPIO_PinState state = enabled ? LED_ON : LED_OFF;
#ifdef INTERNAL_LED_RED_GPIO_Port
    HAL_GPIO_WritePin(INTERNAL_LED_RED_GPIO_Port, INTERNAL_LED_RED_Pin, state);
#else
    (void)state;
#endif

#ifdef EXT_RGB_LED_RED_GPIO_Port
    HAL_GPIO_WritePin(EXT_RGB_LED_RED_GPIO_Port, EXT_RGB_LED_RED_Pin, state);
#endif
}
static void write_green(bool enabled) {
    GPIO_PinState state = enabled ? LED_ON : LED_OFF;
#ifdef INTERNAL_LED_GREEN_GPIO_Port
    HAL_GPIO_WritePin(INTERNAL_LED_GREEN_GPIO_Port, INTERNAL_LED_GREEN_Pin, state);
#else
    (void)state;
#endif


#ifdef EXT_RGB_LED_GREEN_GPIO_Port
    HAL_GPIO_WritePin(EXT_RGB_LED_GREEN_GPIO_Port, EXT_RGB_LED_GREEN_Pin, state);
#endif
}
static void write_blue(bool enabled) {
    GPIO_PinState state = enabled ? LED_ON : LED_OFF;
#ifdef INTERNAL_LED_BLUE_GPIO_Port
    HAL_GPIO_WritePin(INTERNAL_LED_BLUE_GPIO_Port, INTERNAL_LED_BLUE_Pin, state);
#else
    (void)state;
#endif

#ifdef EXT_RGB_LED_BLUE_GPIO_Port
    HAL_GPIO_WritePin(EXT_RGB_LED_BLUE_GPIO_Port, EXT_RGB_LED_BLUE_Pin, state);
#endif
}

void Led::set(Color color) {
    auto color_byte = std::byte(static_cast<uint8_t>(color));

    auto red = static_cast<bool>(color_byte & std::byte{1 << 2});
    auto green = static_cast<bool>(color_byte & std::byte{1 << 1});
    auto blue = static_cast<bool>(color_byte & std::byte{1 << 0});

    write_red(red);
    write_green(green);
    write_blue(blue);
}

void Led::reset() {
    write_red(false);
    write_green(false);
    write_blue(false);
}

void Led::blink(Color first, Color second) {
    auto crnt_time_ms = HAL_GetTick();
    auto color = (crnt_time_ms % 1000 > 500) ? first : second;
    set(color);
}


}  // namespace Board
