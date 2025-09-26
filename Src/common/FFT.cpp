
/*
 * Copyright (C) 2024 Stepanova Anastasiia <asiiapine@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <cstring>
#include <cstdio>
#include "FFT.hpp"
#include "logging.hpp"

bool FFT::init(uint16_t window_size, uint16_t num_axes, float sample_rate_hz) {
    if (window_size > FFT_MAX_SIZE) {
        return false;
    }
    n_axes = num_axes;
    rfft_spec = rfft::init_rfft(_hanning_window.data(), _fft_input_buffer.data(),
                          _fft_output_buffer.data(), &window_size);
    size = window_size;
    _sample_rate_hz = sample_rate_hz;
    _resolution_hz =  sample_rate_hz / size;
    fft_max_freq = _sample_rate_hz / 2;
    return true;
}

void FFT::update(float *input) {
    std::array<real_t, MAX_NUM_AXES> conv_input;
    rfft::convert_float_to_real_t(input, conv_input.data(), n_axes);
    //static Logging logger{"FFT"};
    //static char buffer[30];
    for (uint8_t axis = 0; axis < n_axes; axis++) {
        uint16_t &buffer_index = _fft_buffer_index[axis];

        if (buffer_index < size) {
            // convert int16_t -> real_t (scaling isn't relevant)
            data_buffer[axis][buffer_index] = conv_input[axis] / 2;
            buffer_index++;
            _fft_updated[axis] = false;
            continue;
        }

        rfft::apply_hanning_window(data_buffer[axis].data(), _fft_input_buffer.data(),
                                _hanning_window.data(), size);
        rfft::rfft_one_cycle(rfft_spec, _fft_input_buffer.data(), _fft_output_buffer.data());
        find_peaks(axis);

        //snprintf(buffer, sizeof(buffer), "%d, %d %d", axis, (int)_fft_buffer_index[axis],
        //                                                                   (int)is_updated());
        //logger.log_info(buffer);
        // shift buffer (3/4 overlap) as sliding window for input data
        const int overlap_start = size / 4;
        memmove(&data_buffer[axis][0], &data_buffer[axis][overlap_start],
                sizeof(real_t) * overlap_start * 3);
        _fft_buffer_index[axis] = overlap_start * 3;
    }
    _find_dominant();
}

void FFT::find_peaks(uint8_t axis) {
    std::array<float, NUMBER_OF_SAMPLES> fft_output_buffer_float;
    rfft::convert_real_t_to_float(_fft_output_buffer.data(), fft_output_buffer_float.data(), size);

    // sum total energy across all used buckets for SNR
    float bin_mag_sum = 0;
    // calculate magnitudes for each fft bin
    for (uint16_t fft_index = 0; fft_index < size/2; fft_index ++) {
        auto real = rfft::get_real_by_index(fft_output_buffer_float.data(), fft_index);
        auto imag = rfft::get_imag_by_index(fft_output_buffer_float.data(), fft_index);
        const float fft_magnitude = sqrtf(real * real + imag * imag);
        _peak_magnitudes_all[fft_index] = fft_magnitude;
        bin_mag_sum += fft_magnitude;
    }
    std::array<float, MAX_NUM_PEAKS> peak_magnitude;
    std::array<uint16_t, MAX_NUM_PEAKS> raw_peak_index;

    _identify_peaks_bins(peak_magnitude.data(), raw_peak_index.data());
    auto num_peaks_found = _estimate_peaks(peak_magnitude.data(), raw_peak_index.data(),
                                                fft_output_buffer_float.data(), bin_mag_sum, axis);
    // reset values if no peaks found
    _fft_updated[axis] = true;
    if (num_peaks_found == 0) {
        for (int peak_new = 0; peak_new < MAX_NUM_PEAKS; peak_new++) {
            peak_frequencies[axis][peak_new] = 0;
            peak_snr[axis][peak_new] = 0;
            peak_magnitudes[axis][peak_new] = 0;
        }
        return;
    }
}

void FFT::_find_dominant() {
    if (!is_updated()) {
        return;
    }
    dominant_frequency = 0;
    dominant_snr = 0;
    dominant_mag = 0;
    for (uint8_t axis = 0; axis < n_axes; axis++) {
        for (uint8_t peak = 0; peak < MAX_NUM_PEAKS; peak++) {
            if (peak_snr[axis][peak] > dominant_snr) {
                dominant_snr = peak_snr[axis][peak];
                dominant_frequency = peak_frequencies[axis][peak];
                dominant_mag = peak_magnitudes[axis][peak];
            }
        }
    }
    //static char buffer[30];
    //snprintf(buffer, sizeof(buffer), "updated dominant %d", size);
    //static Logging logger{"FFT"};
    //logger.log_info(buffer);
}

void FFT::_identify_peaks_bins(float peak_magnitude[MAX_NUM_PEAKS],
                             uint16_t raw_peak_index[MAX_NUM_PEAKS]) {
    for (uint8_t i = 0; i < MAX_NUM_PEAKS; i++) {
        float largest_peak = 0;
        uint16_t largest_peak_index = 0;

        // Identify i'th peak bin
        for (uint16_t bin_index = 1; bin_index < size/2; bin_index++) {
            float freq_hz = _resolution_hz * bin_index;

            if ((_peak_magnitudes_all[bin_index] > largest_peak)
                && (freq_hz >= fft_min_freq)
                && (freq_hz <= fft_max_freq)) {
                largest_peak_index = bin_index;
                largest_peak = _peak_magnitudes_all[bin_index];
            }
        }

        if (largest_peak_index > 0) {
            raw_peak_index[i] = largest_peak_index;
            peak_magnitude[i] = largest_peak;
            // remove peak + sides (included in frequency estimate later)
            _peak_magnitudes_all[largest_peak_index - 1] = 0;
            _peak_magnitudes_all[largest_peak_index]     = 0;
            _peak_magnitudes_all[largest_peak_index + 1] = 0;
        }
    }
}

uint16_t FFT::_estimate_peaks(float* peak_magnitude,
                                    uint16_t* raw_peak_index,
                                    float* fft,
                                    float magnitudes_sum, uint8_t axis) {
    uint16_t num_peaks_found = 0;
    for (int peak_new = 0; peak_new < MAX_NUM_PEAKS; peak_new++) {
        if (raw_peak_index[peak_new] == 0) {
            continue;
        }
        float adjusted_bin = 0.5f *
                        _estimate_peak_freq(fft, 2 * raw_peak_index[peak_new]);
        if (adjusted_bin > size || adjusted_bin < 0) {
            continue;
        }
        float freq_adjusted = _resolution_hz * adjusted_bin;
        // check if we already found the peak
        bool peak_close = false;
        for (int peak_prev = 0; peak_prev < peak_new; peak_prev++) {
            peak_close = (fabsf(freq_adjusted - peak_frequencies[axis][peak_prev])
                                            < (_resolution_hz * 10.0f));
            if (peak_close) {
                break;
            }
        }
        if (peak_close) {
            continue;
        }
        // snr is in dB
        float snr;
        if (magnitudes_sum - peak_magnitude[peak_new] < 1.0e-19f) {
            if (magnitudes_sum > 0) {
                snr = MIN_SNR;
            } else {
                snr = 0.0f;
            }
        } else {
            snr = 10.f * log10f((size - 1) * peak_magnitude[peak_new] /
                            (magnitudes_sum - peak_magnitude[peak_new]));
        }
        // keep if SNR satisfies the requirement and the frequency is within the range
        if ((snr < MIN_SNR)
            || (freq_adjusted < fft_min_freq)
            || (freq_adjusted > fft_max_freq)) {
                continue;
        }
        peak_frequencies[axis][num_peaks_found] = freq_adjusted;
        peak_magnitudes[axis][num_peaks_found] = peak_magnitude[peak_new];
        peak_snr[axis][num_peaks_found] = snr;
        num_peaks_found++;
    }
    return num_peaks_found;
}

// tau(x) = 1/4 * log(3*x^2 + 6*x + 1) - sqrt(6)/24*log((x + 1 - sqrt(2/3))/(x + 1 + sqrt(2/3)))
static constexpr float tau(float x) {
    float addend_1 = 1/4 * logf(3 * x * x + 6 * x + 1);
    float multiplier_2 = sqrtf(6) / 24;
    float addend_2 = logf((x + 1 - sqrtf(2 / 3)) / (x + 1 + sqrtf(2 / 3)));
    return addend_1 - multiplier_2 * addend_2;
}

// find peak location using Quinn's Second Estimator (2020-06-14: http://dspguru.com/dsp/howtos/how-to-interpolate-fft-peak/)
float FFT::_estimate_peak_freq(float fft[], int peak_index) {
    if (peak_index < 2 || peak_index >= size) {
        return -1;
    }

    std::array<float, 3> real;
    std::array<float, 3> imag;
    for (int i = -1; i < 2; i++) {
        real[i + 1] = rfft::get_real_by_index(fft, peak_index + i);
        imag[i + 1] = rfft::get_imag_by_index(fft, peak_index + i);
    }

    static constexpr int k = 1;

    const float divider = (real[k] * real[k] + imag[k] * imag[k]);
    if (divider < 1.0e-19f) {
        return -1;
    }
    // ap = (X[k + 1].r * X[k].r + X[k+1].i * X[k].i) / (X[k].r * X[k].r + X[k].i * X[k].i)
    float ap = (real[k + 1] * real[k] + imag[k + 1] * imag[k]) / divider;

    // dp = -ap / (1 – ap)
    if (std::fabs(1.0f - ap) < 1.0e-19f) {
        return -1;
    }
    float dp = -ap  / (1.f - ap);

    // am = (X[k - 1].r * X[k].r + X[k – 1].i * X[k].i) / (X[k].r * X[k].r + X[k].i * X[k].i)
    float am = (real[k - 1] * real[k] + imag[k - 1] * imag[k]) / divider;

    // dm = am / (1 – am)
    if (std::fabs(1.f - am) < 1.0e-19f) {
        return -1;
    }
    float dm = am / (1.f - am);

    // d = (dp + dm) / 2 + tau(dp * dp) – tau(dm * dm)
    float d = (dp + dm) / 2.f + tau(dp * dp) - tau(dm * dm);

    // k’ = k + d
    return peak_index + 2.f * d;
}