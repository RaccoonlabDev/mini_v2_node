/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022-2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef SRC_MAIN_HPP_
#define SRC_MAIN_HPP_

#ifdef __cplusplus
extern "C" {
#endif

uint32_t HAL_GetTick();

static inline uint32_t HAL_GetUIDw0() {return 0;}
static inline uint32_t HAL_GetUIDw1() {return 0;}
static inline uint32_t HAL_GetUIDw2() {return 0;}
void HAL_NVIC_SystemReset();

#ifdef __cplusplus
}
#endif

#endif  // SRC_MAIN_HPP_
