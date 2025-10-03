#ifndef SRC_COMMON_OSCILATION_GENERATOR_H_
#define SRC_COMMON_OSCILATION_GENERATOR_H_
#include <cstdint>
#include <vector>

// will ne reused in CheckOnFewWindows tests
#ifndef FIXED_SEED
extern std::mt19937 rd; 
#else
extern std::random_device rd;
#endif

struct InitOneSignParamType   {
    uint16_t sample_rate_hz = 0;
    uint16_t freq_hz = 0;
    uint16_t amplitude = 0;
};

struct InitMultiSignalsParamType   {
    uint16_t sample_rate_hz;
    uint8_t n_signals;
    uint16_t max_freq;
};

class SinSignalGenerator {
public:
    SinSignalGenerator(){}
    explicit SinSignalGenerator(InitOneSignParamType signal_parameters);
    SinSignalGenerator(uint16_t sample_rate_hz);
    SinSignalGenerator(uint16_t sample_rate_hz, uint16_t freq_hz, uint16_t amplitude);
    void setFreq (uint16_t freq);
    void setAmpl (uint16_t ampl);
    float get_next_sample();

    uint16_t freq_hz;
    uint16_t amplitude;

private:
    uint16_t sample_rate_hz;
    float phase = 0;
    float secs = 0;
};

class MultiSignalsSinGenerator {
public:
    uint16_t max_freq;
    uint8_t n_signals;
    uint16_t min_freq = 0;
    uint16_t sample_rate_hz;
    std::vector<SinSignalGenerator> signals_generator;
    std::vector<std::pair<uint16_t, uint16_t>> dominant_sig;

    void init();

    MultiSignalsSinGenerator() = default;

    MultiSignalsSinGenerator(uint8_t n_signals, uint16_t sample_rate_hz, uint16_t max_freq);

    explicit MultiSignalsSinGenerator(InitMultiSignalsParamType parameters);

    float get_next_samples();
};

#endif  // SRC_COMMON_OSCILATION_GENERATOR_H_