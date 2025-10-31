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
#define MIN_SNR         8.0f
#define MAX_NUM_AXES    3
constexpr size_t NUMBER_OF_SAMPLES = 512;

class FFT {
public:
    bool init(uint16_t window_size, uint16_t num_axes, float sample_rate_hz);
    void update(float *input);
    float fft_min_freq = 0;
    float fft_max_freq;
    float _resolution_hz;
    float dominant_frequency;
    float dominant_snr;
    float dominant_mag;
    std::array<std::array<float, MAX_NUM_PEAKS>, MAX_NUM_AXES> peak_frequencies {0};
    std::array<std::array<float, MAX_NUM_PEAKS>, MAX_NUM_AXES> peak_magnitudes {0};
    std::array<std::array<float, MAX_NUM_PEAKS>, MAX_NUM_AXES> peak_snr {0};
    uint32_t total_time;
    bool is_updated() {
        for (uint8_t axis = 0; axis < n_axes; axis++) {
            if (!_fft_updated[axis]) {
                return false;
            }
        }
        return true;
    }

private:
    uint16_t size;
    std::array<bool, MAX_NUM_AXES>          _fft_updated{false};
    std::array<real_t, FFT_MAX_SIZE>        _hanning_window;

    // Will have double size as consist of complex numbers
    // i.e. The input array is packed with all the real numbers.
    // The output array is for the complex numbers without length reduced
    std::array<real_t, FFT_MAX_SIZE * 2>    _fft_output_buffer;
    std::array<real_t, FFT_MAX_SIZE>        _fft_input_buffer;
    std::array<float, NUMBER_OF_SAMPLES>    _peak_magnitudes_all;
    std::array<uint16_t, MAX_NUM_AXES>      _fft_buffer_index;
    std::array<std::array<real_t, NUMBER_OF_SAMPLES>, MAX_NUM_AXES> data_buffer;

    uint8_t n_axes;
    float _sample_rate_hz;

    void find_peaks(uint8_t axis);
    float _estimate_peak_freq(float fft[], int peak_index);
    void _find_dominant();
    void _identify_peaks_bins(float peak_magnitude[MAX_NUM_PEAKS],
                             uint16_t raw_peak_index[MAX_NUM_PEAKS]);
    uint16_t _estimate_peaks(float* peak_magnitude,
                                    uint16_t* raw_peak_index,
                                    float* fft_output_buffer_float,
                                    float bin_mag_sum, uint8_t axis);
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
