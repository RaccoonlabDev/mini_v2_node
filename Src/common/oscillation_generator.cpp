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
    std::uniform_int_distribution<int> dist(0, static_cast<int>(max_freq));

    for (int j = 0; j < n_signals; j++) {
        int range = static_cast<int>(max_freq) - static_cast<int>(min_freq);
        if (range < 0) range = 0;
        int freq_hz_val = static_cast<int>(min_freq) + (dist(rd) % range);
        int amplitude_val = 1 + (dist(rd) % 100);
        
        uint16_t freq_hz = static_cast<uint16_t>(freq_hz_val);
        uint16_t amplitude = static_cast<uint16_t>(amplitude_val);

        signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);
        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
            dominant_sig.insert(dominant_sig.begin(), {amplitude, freq_hz});
        }
    }
    // sort by amplitude value. Usual sort returned warning -Werror=strict-overflow, so i changed it on safer approach
    std::stable_sort(dominant_sig.begin(), dominant_sig.end(), 
    [](const std::pair<uint16_t, uint16_t>& a, const std::pair<uint16_t, uint16_t>& b) {
        return std::less<uint16_t>{}(a.first, b.first);  
    });
}


MultiSignalsSinGenerator::MultiSignalsSinGenerator(uint8_t n_sig, uint16_t sample_rate, uint16_t max_f) :
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
    for (int j = 0; j < n_signals; j++) {
        sample +=signals_generator[j].get_next_sample();
    }
    return sample;
}
