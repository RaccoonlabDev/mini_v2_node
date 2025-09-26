#include <random>
#include <algorithm>  // For std::clamp
#include <cstdlib>
#include <cmath>  // For std::fabs
#include <tuple>  // for tuple

#include "oscillation_generator.hpp"

std::random_device rd;
SinSignalGenerator::SinSignalGenerator(InitOneSignParamType signal_parameters) :
                                    sample_rate_hz(signal_parameters.sample_rate_hz),
                                    freq_hz(signal_parameters.freq_hz),
                                    amplitude(signal_parameters.amplitude) {}
SinSignalGenerator::SinSignalGenerator(float sample_rate_hz, float freq_hz, float amplitude) :
                                    sample_rate_hz(sample_rate_hz),
                                    freq_hz(freq_hz),
                                    amplitude(amplitude) {}
float SinSignalGenerator::get_next_sample() {
    auto sin = sinf(2 * M_PI * freq_hz * secs + phase);
    float sample = amplitude * sin;
    secs += 1 / sample_rate_hz;
    return sample;
}


void MultiSignalsSinGenerator::init() {
    signals_generator.resize(n_signals);
    uint16_t max_amplitude = 0;
    std::uniform_int_distribution<int> dist(0, max_freq);

    for (int j = 0; j < n_signals; j++) {
        uint16_t freq_hz = dist(rd) % (max_freq - min_freq) + min_freq;
        uint16_t amplitude = 1 + dist(rd) % 100;
        signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);
        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
            dominant_sig.insert(dominant_sig.begin(), std::make_tuple(amplitude, freq_hz));
        }
    }
    // sort by amplitude value
    std::sort(dominant_sig.begin(), dominant_sig.end());
}


MultiSignalsSinGenerator::MultiSignalsSinGenerator(uint8_t n_signals, uint16_t sample_rate_hz, uint16_t max_freq) :
                                            n_signals(n_signals),
                                            sample_rate_hz(sample_rate_hz),
                                            max_freq(max_freq) {
    init();
}

MultiSignalsSinGenerator::MultiSignalsSinGenerator(InitMultiSignalsParamType parameters) :
                                            n_signals(parameters.n_signals),
                                            sample_rate_hz(parameters.sample_rate_hz),
                                            max_freq(parameters.max_freq) {
    init();
}

float MultiSignalsSinGenerator::get_next_samples() {
    float sample = 0;
    for (int j = 0; j < n_signals; j++) {
        sample +=signals_generator[j].get_next_sample();
    }
    return sample;
}
