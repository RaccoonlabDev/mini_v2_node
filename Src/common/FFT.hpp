/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SRC_COMMON_ALGORITHMS_FFT_H_
#define SRC_COMMON_ALGORITHMS_FFT_H_

#include <array>
#include "rfft.hpp"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FFT_MAX_SIZE    512
#define MAX_NUM_PEAKS   3
#define MIN_SNR         1.0f
#define MAX_NUM_AXES    3
constexpr size_t NUMBER_OF_SAMPLES = 512;

class FFT {
public:
    bool init(uint16_t window_size, uint16_t num_axes, float sample_rate_hz);
    void update(float *input);
    float fft_min_freq = 0;
    float fft_max_freq;
    float _resolution_hz;
    float peak_frequencies [MAX_NUM_AXES][MAX_NUM_PEAKS] {0};
    float peak_snr [MAX_NUM_AXES][MAX_NUM_PEAKS] {0};
    bool _fft_updated[MAX_NUM_AXES]{false};
    uint32_t total_time;

private:
    uint16_t size;
    real_t *_hanning_window{nullptr};
    real_t *_fft_output_buffer{nullptr};
    real_t *_fft_input_buffer{nullptr};
    std::array<std::array<real_t, NUMBER_OF_SAMPLES>, MAX_NUM_AXES> data_buffer;
    std::array<float, NUMBER_OF_SAMPLES> _peak_magnitudes_all;

    uint16_t _fft_buffer_index[3] {};
    uint8_t n_axes;
    float _sample_rate_hz;

    float estimate_peak_freq(float fft[], int peak_index);
    void find_peaks(uint8_t axis);
    #ifdef HAL_MODULE_ENABLED
        // specification of arm_rfft_instance_q15
        // https://arm-software.github.io/CMSIS_5/DSP/html/group__RealFFT.html
        arm_rfft_instance_q15 rfft_spec;
    #else
        // plan for the r2c transform from fftw3 library.
        fftw_plan rfft_spec;
    #endif
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_ALGORITHMS_FFT_H_
