#include "main.h" // For HAL
#include <random>
#include <algorithm>  // For std::clamp
#include <cstdlib>
#include <cmath>  // For std::fabs
#include <tuple>  // for tuple

#include "oscillation_generator.hpp"

// TODO(ilyha_dev): make rd for tests random and for HAL make HAL_RNG_GenerateRandomNumber
// Note! arm compiler doesn't support random device, so defining it in dronecan_vX will lead to errors
#ifndef FIXED_SEED
std::mt19937 rd(12345);
#else
std::random_device rd;
#endif

//uint32_t get_hw_random() {
//    uint32_t random_number;
//    HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
//    return random_number;
//}

SinSignalGenerator::SinSignalGenerator(InitOneSignParamType signal_parameters) :
                                    freq_hz(signal_parameters.freq_hz),
                                    amplitude(signal_parameters.amplitude),
                                    sample_rate_hz(signal_parameters.sample_rate_hz) {};
                                    
SinSignalGenerator::SinSignalGenerator(uint16_t sample_rate, uint16_t freq, uint16_t ampl) :
                                    freq_hz(freq),
                                    amplitude(ampl),
                                    sample_rate_hz(sample_rate) {};
SinSignalGenerator::SinSignalGenerator(uint16_t sample_rate) :
                                    sample_rate_hz(sample_rate) {
                                        freq_hz = 0;
                                        amplitude = 0;
                                    };
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

void MultiSignalsSinGenerator::init() {
    signals_generator.resize(n_signals);
    uint16_t max_amplitude = 0;
    
    for (size_t j = 0; j < n_signals; j++) {
        /*std::uniform_int_distribution<uint16_t> freq_dist(min_freq, max_freq);
        std::uniform_int_distribution<uint16_t> amp_dist(1, 100);*/
        
        uint16_t freq_hz = 3;//freq_dist(rd);
        uint16_t amplitude = 4;//amp_dist(rd);
        
        signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);
        
        /*if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
            dominant_sig.insert(dominant_sig.begin(), {amplitude, freq_hz});
        }*/
    }
    
    /*std::stable_sort(dominant_sig.begin(), dominant_sig.end(),
        [](const std::pair<uint16_t, uint16_t>& a, const std::pair<uint16_t, uint16_t>& b) {
            return std::less<uint16_t>{}(a.first, b.first);
        });*/
}


MultiSignalsSinGenerator::MultiSignalsSinGenerator(size_t n_sig, uint16_t sample_rate, uint16_t max_f) :
                                            max_freq(max_f),
                                            n_signals(n_sig),
                                            sample_rate_hz(sample_rate) {
    init();
}

MultiSignalsSinGenerator::MultiSignalsSinGenerator(InitMultiSignalsParamType parameters) :
                                            max_freq(parameters.max_freq),
                                            n_signals(parameters.n_signals),
                                            sample_rate_hz(parameters.sample_rate_hz) {
    init();
}

float MultiSignalsSinGenerator::get_next_samples() {
    float sample = 0;
    for (size_t j = 0; j < n_signals; j++) {
        sample +=signals_generator[j].get_next_sample();
    }
    return sample;
}
