/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include <gtest/gtest.h>
#include <cmath>  // For std::fabs
#include <algorithm>  // For std::clamp
#include <tuple>  // for tuple

#include "FFT.hpp"

unsigned int seed = 1;

template <typename T>
auto IsInRange(T lo, T hi) {
    return AllOf(Ge((lo)), Le((hi)));
}
::testing::AssertionResult IsBetweenInclusive(int val, int a, int b)
{
    if((val >= a) && (val <= b))
        return ::testing::AssertionSuccess();
    else
        return ::testing::AssertionFailure()
               << val << " is outside the range " << a << " to " << b;
}

struct InitFFTParamType   {
    float sample_rate_hz;
    uint8_t n_axes;
    uint16_t window_size;
};

template <typename T>
struct InitParamType   {
    InitFFTParamType fft_parameters;
    T signals_parameters;
};

struct InitOneSignParamType   {
    float sample_rate_hz = 0;
    float freq_hz = 0;
    float amplitude = 0;
};

struct InitMultiSignalsParamType   {
    uint16_t sample_rate_hz;
    uint8_t n_signals;
    uint16_t max_freq;
};

struct InitParamOneSignalWithRes {
    InitParamType<InitOneSignParamType> parameters;
    bool result;
};

struct InitParamMultiSignalWithRes {
    InitParamType<InitMultiSignalsParamType> parameters;
    bool result;
};

InitParamOneSignalWithRes OneSignalTestParams[7] = {
    // 0
    {{InitFFTParamType{      .sample_rate_hz = 100,  .n_axes   = 1,   .window_size  = 50},
      InitOneSignParamType{  .sample_rate_hz = 100,  .freq_hz  = 3,   .amplitude    = 10}},
      true},
    // 1
    {{InitFFTParamType{      .sample_rate_hz = 1000, .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1000, .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 2
    {{InitFFTParamType{      .sample_rate_hz = 1000, .n_axes   = 1,   .window_size  = 100},
      InitOneSignParamType{  .sample_rate_hz = 1000, .freq_hz  = 5,   .amplitude    = 10}},
      true},
    // 3
    {{InitFFTParamType{      .sample_rate_hz = 512,  .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 512,  .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 4
    {{InitFFTParamType{      .sample_rate_hz = 1024, .n_axes   = 3,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1024, .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 5
    {{InitFFTParamType{      .sample_rate_hz = 256,  .n_axes   = 3,   .window_size  = 256},
      InitOneSignParamType{  .sample_rate_hz = 256,  .freq_hz  = 100, .amplitude    = 1}},
      true},
    // 6
    {{InitFFTParamType{     .sample_rate_hz  = 300,   .n_axes   = 1,  .window_size  = 200},
      InitOneSignParamType{ .sample_rate_hz = 1000,  .freq_hz  = 100, .amplitude    = 1}},
      false},
};

InitParamMultiSignalWithRes MultiSignalTestParams[8] = {
    // 0
    {{InitFFTParamType{          .sample_rate_hz = 24,   .n_axes    = 1,   .window_size = 24},
      InitMultiSignalsParamType{ .sample_rate_hz = 24,   .n_signals = 2,   .max_freq    = 12}},
      true},
    // 1
    {{InitFFTParamType{          .sample_rate_hz = 512, .n_axes    = 1,   .window_size = 512},
      InitMultiSignalsParamType{ .sample_rate_hz = 512, .n_signals = 5,   .max_freq    = 256}},
      true},
    // 2
    {{InitFFTParamType{          .sample_rate_hz = 1000, .n_axes    = 1,   .window_size = 100},
      InitMultiSignalsParamType{ .sample_rate_hz = 1000, .n_signals = 4,   .max_freq    = 50}},
      true},
    // 3
    {{InitFFTParamType{          .sample_rate_hz = 2000, .n_axes    = 1,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 2000, .n_signals = 10,   .max_freq   = 128}},
      true},
    // 4
    {{InitFFTParamType{          .sample_rate_hz = 2000, .n_axes    = 1,   .window_size = 200},
      InitMultiSignalsParamType{ .sample_rate_hz = 2000, .n_signals = 12,   .max_freq   = 100}},
      true},
    // 5
    {{InitFFTParamType{          .sample_rate_hz = 1024, .n_axes    = 1,   .window_size = 512},
      InitMultiSignalsParamType{ .sample_rate_hz = 1024, .n_signals = 15,   .max_freq   = 256}},
      true},
    // 6
    {{InitFFTParamType{          .sample_rate_hz = 256, .n_axes     = 3,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 256, .n_signals  = 13,   .max_freq   = 128}},
      true},
    // 7
    {{InitFFTParamType{          .sample_rate_hz = 512, .n_axes     = 3,   .window_size = 512},
      InitMultiSignalsParamType{ .sample_rate_hz = 512, .n_signals  = 10,   .max_freq   = 256}},
      true},
};

class SinSignalGenerator {
public:
    SinSignalGenerator(){}
    explicit SinSignalGenerator(InitOneSignParamType signal_parameters) {
        this->sample_rate_hz = signal_parameters.sample_rate_hz;
        this->freq_hz = signal_parameters.freq_hz;
        this->amplitude = signal_parameters.amplitude;
        this->phase = 0;
        this->secs = 0;
    }
    SinSignalGenerator(float sample_rate_hz, float freq_hz, float amplitude) {
        this->sample_rate_hz = sample_rate_hz;
        this->freq_hz = freq_hz;
        this->amplitude = amplitude;
        this->phase = 0;
        this->secs = 0;
    }
    float get_next_sample() {
        auto sin = sinf(2 * M_PI * freq_hz * secs + phase);
        float sample = (float)amplitude * sin;
        secs += 1 / sample_rate_hz;
        return sample;
    }
    float freq_hz;
    float amplitude;

private:
    float sample_rate_hz;
    float phase;
    float secs;
};

class MultiSignalsSinGenerator {
public:
    std::vector<SinSignalGenerator> signals_generator;
    uint8_t n_signals;
    std::vector<std::tuple<uint16_t, uint16_t>> dominant_sig;

    void init() {
        signals_generator.resize(n_signals);
        uint16_t max_amplitude = 0;
        for (int j = 0; j < n_signals; j++) {
            uint16_t freq_hz = rand_r(&seed) % (max_freq - min_freq) + min_freq;
            uint16_t amplitude = 1 + rand_r(&seed) % 100;
            signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);
            if (amplitude > max_amplitude) {
                max_amplitude = amplitude;
                dominant_sig.insert(dominant_sig.begin(), std::make_tuple(amplitude, freq_hz));
            }
        }
        for (int j = 0; j < dominant_sig.size(); j++) {
            printf("dominant freq: %d\n", std::get<1>(dominant_sig[j]));
        }
        // sort by amplitude value
        std::sort(dominant_sig.begin(), dominant_sig.end());
    }

    MultiSignalsSinGenerator() {}

    MultiSignalsSinGenerator(uint8_t n_signals, uint16_t sample_rate_hz, uint16_t max_freq) {
        this->n_signals = n_signals;
        this->sample_rate_hz = sample_rate_hz;
        this->max_freq = max_freq;
        init();
    }

    explicit MultiSignalsSinGenerator(InitMultiSignalsParamType parameters) {
        this->n_signals = parameters.n_signals;
        this->sample_rate_hz = parameters.sample_rate_hz;
        this->max_freq = parameters.max_freq;
        init();
    }

    float get_next_samples() {
        float sample = 0;
        for (int j = 0; j < n_signals; j++) {
            sample +=signals_generator[j].get_next_sample();
        }
        return sample;
    }

private:
    uint16_t max_freq;
    uint16_t min_freq = 0;
    uint16_t sample_rate_hz;
};

template<typename T, typename InitParamType>
class TestFFTBase : public ::testing::Test {
public:
    /* data */
    FFT fft;
    std::vector<T> signals_generator;
    InitFFTParamType fft_parameters;
    std::vector<InitParamType> signals_parameters;
    float abs_error;

    void print_fft_parameters() {
        // Print the FFT parameters
        std::cout << "FFT Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << fft_parameters.sample_rate_hz << std::endl;
        std::cout << "  Window Size: " << fft_parameters.window_size << std::endl;
        std::cout << "  Number of Axes: " << static_cast<int>(fft_parameters.n_axes) << std::endl;
    }

    void init() {
        fft.init(fft_parameters.window_size, fft_parameters.n_axes, fft_parameters.sample_rate_hz);
        signals_generator.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            auto signal_parameters = signals_parameters[i];
            signals_generator[i] = T(signal_parameters);
        }
        abs_error = 10 * fft._resolution_hz;
    }
};

class TestFFTOneSignalParametrized : public TestFFTBase<SinSignalGenerator,
                                            InitOneSignParamType>,
                        public testing::WithParamInterface<InitParamOneSignalWithRes> {
public:
    /* data */
    bool result;
    bool is_heat_all_peaks{true};

    void print_signal_parameters() {
                // Print the signal parameters
        std::cout << "Signal Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << signals_parameters[0].sample_rate_hz
                    << std::endl;
        std::cout << "  Frequency (Hz): " << signals_parameters[0].freq_hz
                    << std::endl;
        std::cout << "  Amplitude: " << signals_parameters[0].amplitude
                    << std::endl;
    }

    void print_fft_results() {
        printf("fft resolution: %f\n", fft._resolution_hz);
        for (int axis = 0; axis < fft_parameters.n_axes; axis++) {
            printf("AXIS %d\n", axis);
            for (int peak_index = 0; peak_index < MAX_NUM_PEAKS; peak_index++) {
                printf("peak index: %d\n", peak_index);
                printf("fft peak freq: %f\n", fft.peak_frequencies[axis][peak_index]);
                printf("fft peak snr: %f\n", fft.peak_snr[axis][peak_index]);
            }
        }
    }

    void check_values () {
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            bool heat_peak = false;
            for (int j = 0; j < MAX_NUM_PEAKS; j++) {
                auto in_range = IsBetweenInclusive(fft.peak_frequencies[i][j],
                            signals_parameters[i].freq_hz - abs_error,
                            signals_parameters[i].freq_hz + abs_error);
                if (in_range) {
                    heat_peak = true;
                    break;
                }
            }
            is_heat_all_peaks = is_heat_all_peaks && heat_peak;
        }
    }

    void SetUp() override {
        auto parameters = GetParam();
        InitParamType<InitOneSignParamType> init_parameters = parameters.parameters;
        result = parameters.result;
        fft_parameters = init_parameters.fft_parameters;
        // expand signal_parameters to multiple axes if needed
        signals_parameters.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            signals_parameters[i] = init_parameters.signals_parameters;
        }
        print_signal_parameters();
        print_fft_parameters();
        init();
    }
};

TEST_P(TestFFTOneSignalParametrized, CheckOnWindow) {
    float input[fft_parameters.n_axes];
    for (int i = 0; i < fft_parameters.window_size + 1; i++) {
        for (int j = 0; j < fft_parameters.n_axes; j++) {
            input[j] = signals_generator[j].get_next_sample();
        }
        fft.update(input);
    }
    print_fft_results();
    check_values();
    if (result) {
        EXPECT_TRUE(is_heat_all_peaks);
    } else {
        ASSERT_FALSE(is_heat_all_peaks);
    }
}

TEST_P(TestFFTOneSignalParametrized, CheckOnFewWindows) {
    float input[fft_parameters.n_axes];
    auto n_updates = (rand_r(&seed) % 8 + 2) * fft_parameters.window_size;
    // auto n_updates = (2 + rand_r(&seed) % 10) * fft_parameters.window_size;
    for (int i = 0; i < n_updates + 10; i++) {
        for (int j = 0; j < fft_parameters.n_axes; j++) {
            input[j] = signals_generator[j].get_next_sample();
        }
        fft.update(input);
    }
    print_fft_results();
    check_values();
    if (result) {
        EXPECT_TRUE(is_heat_all_peaks);
    } else {
        ASSERT_FALSE(is_heat_all_peaks);
    }
}

INSTANTIATE_TEST_SUITE_P(TestFFTOneSignalParametrized,
                         TestFFTOneSignalParametrized,
                         ::testing::ValuesIn(
                             OneSignalTestParams)
                         );


class TestFFTOnMultiSignalsParametrized : public TestFFTBase<MultiSignalsSinGenerator,
                                                 InitMultiSignalsParamType>,
                public ::testing::WithParamInterface<InitParamMultiSignalWithRes> {
public:
    /* data */
    bool result;
    std::vector<float> input;
    void print_signal_parameters() {
                // Print the signal parameters
        std::cout << "Signal Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << signals_parameters[0].sample_rate_hz
                    << std::endl;
        std::cout << "  Number of Signals: " << signals_parameters[0].n_signals
                    << std::endl;
        std::cout << "  Max Freq: " << signals_parameters[0].max_freq
                    << std::endl;
    }

    void SetUp() override {
        auto parameters = GetParam();
        InitParamType<InitMultiSignalsParamType> init_parameters = parameters.parameters;
        result = parameters.result;
        fft_parameters = init_parameters.fft_parameters;
        signals_parameters.resize(fft_parameters.n_axes);
        input.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            signals_parameters[i] = init_parameters.signals_parameters;
        }
        print_signal_parameters();
        print_fft_parameters();
        init();
    }
};

TEST_P(TestFFTOnMultiSignalsParametrized, CheckOnWindow) {
    for (int i = 0; i < fft_parameters.window_size + 10; i++) {
        for (int j = 0; j < fft_parameters.n_axes; j++) {
            input[j] = signals_generator[j].get_next_samples();
        }
        fft.update(input.data());
    }
    for (int axis = 0; axis < fft_parameters.n_axes; axis++) {
        bool heat_peak;
        auto signal_generator = signals_generator[axis];
        auto n_dominants = signal_generator.dominant_sig.size();
        auto n_signals = signal_generator.n_signals;
        printf("abs error: %f\n", abs_error);
        for (int peak_index = 0; peak_index < MAX_NUM_PEAKS; peak_index++) {
            printf("peak index: %d\n", peak_index);
            printf("fft peak freq: %f\n", fft.peak_frequencies[axis][peak_index]);
            for (int j = 0; j < n_dominants; j++) {
                auto dominant_freq = signal_generator.dominant_sig[j];
                printf("dominant freq: %d\n", std::get<1>(dominant_freq));
                if (IsBetweenInclusive(fft.peak_frequencies[axis][peak_index],
                            std::get<1>(dominant_freq) - abs_error,
                            std::get<1>(dominant_freq) + abs_error)) {
                    heat_peak = true;
                    break;
                }
            }
            if (heat_peak) {
                break;
            }
        }
        if (result) {
            EXPECT_TRUE(heat_peak);
        } else {
            ASSERT_FALSE(heat_peak);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(TestFFTOnMultiSignalsParametrized,
                         TestFFTOnMultiSignalsParametrized,
                         ::testing::ValuesIn(
                             MultiSignalTestParams)
                         );

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
