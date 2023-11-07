/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef SRC_APPLICATION_PERIPHERY_PWM_HPP_
#define SRC_APPLICATION_PERIPHERY_PWM_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note PWM pinout related to RaccoonLab Mini v2 node
 */
enum class PwmPin {
    PWM_1,      // PB7
    PWM_2,      // PB6
    PWM_3,      // PB4
    PWM_4,      // PB5
    PWM_AMOUNT,
};


class PwmPeriphery {
public:
    static int8_t init(PwmPin pin);
    static void set_duration(const PwmPin pin, uint32_t duration_us);
    static uint32_t get_duration(PwmPin pin);
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_APPLICATION_PERIPHERY_PWM_HPP_
