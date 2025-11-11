#ifndef SRC_COMMON_OSCILATION_GENERATOR_H_
#define SRC_COMMON_OSCILATION_GENERATOR_H_
#include <cstdint>
#include <random>

struct InitOneSignParamType   {
    uint16_t sample_rate_hz = 0;
    uint16_t freq_hz = 0;
    uint16_t amplitude = 0;
};

class SinSignalGenerator {
public:
    SinSignalGenerator(){}
    explicit SinSignalGenerator(InitOneSignParamType signal_parameters);
    explicit SinSignalGenerator(uint16_t sample_rate_hz);
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

#endif  // SRC_COMMON_OSCILATION_GENERATOR_H_
