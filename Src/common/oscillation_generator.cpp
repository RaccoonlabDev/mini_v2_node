/*
 * Copyright (C) 2024 Stepanova Anastasiia <asiiapine@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * Preprocess on random seed collaborator Ilia Kliantsevich <iliawork112005@gmail.com>
 */
#include "oscillation_generator.hpp"

#include <cmath>      // For std::fabs
#include <cstdlib>
#include <algorithm>  // For std::clamp
#include <tuple>      // for tuple

#include "main.h"     // For HAL

// TODO(ilyha_dev): make seed for mt19937 be chosen by HAL_RNG
// Note! arm compiler doesn't support random device,
// so defining it in dronecan_vX will lead to errors
#ifndef FIXED_SEED
std::mt19937 rd(12345);
#else
std::random_device rd;
#endif

SinSignalGenerator::SinSignalGenerator(InitOneSignParamType signal_parameters) :
                                    freq_hz(signal_parameters.freq_hz),
                                    amplitude(signal_parameters.amplitude),
                                    sample_rate_hz(signal_parameters.sample_rate_hz) {}

SinSignalGenerator::SinSignalGenerator(uint16_t sample_rate, uint16_t freq, uint16_t ampl) :
                                    freq_hz(freq),
                                    amplitude(ampl),
                                    sample_rate_hz(sample_rate) {}
SinSignalGenerator::SinSignalGenerator(uint16_t sample_rate) :
                                    sample_rate_hz(sample_rate) {
                                        freq_hz = 0;
                                        amplitude = 0;
                                    }
float SinSignalGenerator::get_next_sample() {
    volatile auto sin = sinf(2 * M_PI * freq_hz * secs + phase);
    float sample = amplitude * sin;
    secs += 1.0f / sample_rate_hz;
    return sample;
}
void SinSignalGenerator::setFreq (uint16_t freq) {
    this->freq_hz = freq;
}

void SinSignalGenerator::setAmpl (uint16_t ampl) {
    this->amplitude = ampl;
}


