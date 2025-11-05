/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#include <gtest/gtest.h>
#include <random>
#include <algorithm>  // For std::clamp
#include <cstdlib>
#include <cmath>  // For std::fabs
#include <tuple>  // for tuple
#include "FFT.hpp"
#include "oscillation_generator.hpp" 

class MultiSignalsSinGenerator {
public:
    uint16_t max_freq;
    size_t n_signals;
    uint16_t min_freq = 0;
    uint16_t sample_rate_hz;
    std::vector<SinSignalGenerator> signals_generator;
    std::vector<std::pair<float, float>> dominant_sig;

    void init() {
        signals_generator.resize(n_signals);
        uint16_t max_amplitude = 0;

        for (size_t j = 0; j < n_signals; j++) {
            std::uniform_int_distribution<uint16_t> freq_dist(min_freq, max_freq);
            std::uniform_int_distribution<uint16_t> amp_dist(1, 100);

            uint16_t freq_hz = freq_dist(rd);
            uint16_t amplitude = amp_dist(rd);

            signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);

            if (amplitude > max_amplitude) {
                max_amplitude = amplitude;
                dominant_sig.emplace_back(amplitude, freq_hz);
            }
        }

        std::stable_sort(dominant_sig.begin(), dominant_sig.end(),
            [](const std::pair<uint16_t, uint16_t>& a, const std::pair<uint16_t, uint16_t>& b) {
                return std::less<uint16_t>{}(a.first, b.first);
            });
    }
    MultiSignalsSinGenerator() = default;
    
    MultiSignalsSinGenerator
        (size_t n_sig, uint16_t sample_rate, uint16_t max_f) :
                                                max_freq(max_f),
                                                n_signals(n_sig),
                                                sample_rate_hz(sample_rate) {
        init();
    }

    MultiSignalsSinGenerator(InitMultiSignalsParamType parameters) :
                                                max_freq(parameters.max_freq),
                                                n_signals(parameters.n_signals),
                                                sample_rate_hz(parameters.sample_rate_hz) {
        init();
    }

    float get_next_samples() {
        float sample = 0;
        for (size_t j = 0; j < n_signals; j++) {
            sample +=signals_generator[j].get_next_sample();
        }
        return sample;
    }
};


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

struct InitParamOneSignalWithRes {
    InitParamType<InitOneSignParamType> parameters;
    bool result;
};

struct InitParamMultiSignalWithRes {
    InitParamType<InitMultiSignalsParamType> parameters;
    bool result;
};


template<typename T, typename InitParamType>
class TestFFTBase : public ::testing::Test {
public:
    FFT fft;
    std::vector<T> signals_generator;
    InitFFTParamType fft_parameters;
    std::vector<InitParamType> signals_parameters;
    float abs_error;

    void print_fft_parameters() {
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
    bool result;
    bool is_heat_all_peaks{true};

    void print_signal_parameters() {
        std::cout << "Signal Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << signals_parameters[0].sample_rate_hz
                    << std::endl;
        std::cout << "  Frequency (Hz): " << signals_parameters[0].freq_hz
                    << std::endl;
        std::cout << "  Amplitude: " << signals_parameters[0].amplitude
                    << std::endl;
    }

    void print_fft_results() {
        std::cout <<"fft resolution: " << fft._resolution_hz << "\n";
        for (int axis = 0; axis < fft_parameters.n_axes; axis++) {
            std::cout <<"AXIS " << axis << "\n";
            for (int peak_index = 0; peak_index < MAX_NUM_PEAKS; peak_index++) {
                std::cout <<"peak index: " << peak_index << "\n";
                std::cout <<"fft peak freq: " << fft.peak_frequencies[axis][peak_index] << "\n";
                std::cout <<"fft peak snr: " << fft.peak_snr[axis][peak_index] << "\n";
            }
        }
    }

    void check_axis(int axis) {
        bool heat_peak = false;
        for (int j = 0; j < MAX_NUM_PEAKS; j++) {
            auto in_range = IsBetweenInclusive(fft.peak_frequencies[axis][j],
                        signals_parameters[axis].freq_hz - abs_error,
                        signals_parameters[axis].freq_hz + abs_error);
            if (in_range) {
                heat_peak = true;
                break;
            }
        }
        if (result) {
            EXPECT_TRUE(heat_peak);
        } else {
            ASSERT_FALSE(heat_peak);
        }
    }

    void SetUp() override {
        InitParamType<InitOneSignParamType> init_parameters = GetParam().parameters;
        result = GetParam().result;
        fft_parameters = init_parameters.fft_parameters;
        // expand signal_parameters to multiple axes if needed
        signals_parameters.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            signals_parameters[i] = init_parameters.signals_parameters;
        }
        // To print FFT parameters for debug uncomment the following line
        print_signal_parameters();
        print_fft_parameters();
        init();
    }
};

const std::array<InitParamOneSignalWithRes, 7> OneSignalTestParams = {
{
    // 0
    {{InitFFTParamType{      .sample_rate_hz = 100,  .n_axes   = 1,   .window_size  = 50},
      InitOneSignParamType{  .sample_rate_hz = 100,  .freq_hz  = 3,   .amplitude    = 10}},
      true},
    // 1
    {{InitFFTParamType{      .sample_rate_hz = 1000, .n_axes   = 1,   .window_size  = 100},
      InitOneSignParamType{  .sample_rate_hz = 1000, .freq_hz  = 5,   .amplitude    = 10}},
      true},
    // 2
    {{InitFFTParamType{      .sample_rate_hz = 512,  .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 512,  .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 3
    {{InitFFTParamType{      .sample_rate_hz = 1000, .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1000, .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 4
    {{InitFFTParamType{      .sample_rate_hz = 1024, .n_axes   = 3,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1024, .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 5
    {{InitFFTParamType{      .sample_rate_hz = 256,  .n_axes   = 3,   .window_size  = 256},
      InitOneSignParamType{  .sample_rate_hz = 256,  .freq_hz  = 200, .amplitude    = 1}},
      false},
    // 6
    {{InitFFTParamType{     .sample_rate_hz  = 100,   .n_axes   = 1,  .window_size  = 200},
      InitOneSignParamType{ .sample_rate_hz = 1000,  .freq_hz  = 100, .amplitude    = 1}},
      false},
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
    // To print FFT results for debug uncomment the following line
    // print_fft_results();
    for (int axis = 0; axis < fft_parameters.n_axes; axis++) {
        check_axis(axis);
    }
}

TEST_P(TestFFTOneSignalParametrized, CheckOnFewWindows) {
    float input[fft_parameters.n_axes];
    std::uniform_int_distribution<int> dist(0, 8);
    auto n_updates = (dist(rd) % 8 + 2) * fft_parameters.window_size;
    for (int i = 0; i < n_updates + 10; i++) {
        for (int j = 0; j < fft_parameters.n_axes; j++) {
            input[j] = signals_generator[j].get_next_sample();
        }
        fft.update(input);
    }
    // To print FFT results for debug uncomment the following line
    // print_fft_results();
    for (int axis = 0; axis < fft_parameters.n_axes; axis++) {
        check_axis(axis);
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
    bool result;
    std::vector<float> input;
    void print_signal_parameters() {
        std::cout << "Signal Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << signals_parameters[0].sample_rate_hz
                    << std::endl;
        std::cout << "  Number of Signals: " << (int)signals_parameters[0].n_signals
                    << std::endl;
        std::cout << "  Max Freq: " << signals_parameters[0].max_freq
                    << std::endl;
    }

    void SetUp() override {
        InitParamType<InitMultiSignalsParamType> init_parameters = GetParam().parameters;
        result = GetParam().result;
        fft_parameters = init_parameters.fft_parameters;
        signals_parameters.resize(fft_parameters.n_axes);
        input.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            signals_parameters[i] = init_parameters.signals_parameters;
        }
        // To print FFT parameters for debug uncomment the following line
        // print_signal_parameters();
        // print_fft_parameters();
        init();
    }

    
    void check_axis(int axis) {
        bool heat_peak {false};
        auto signal_generator = signals_generator[axis];
        auto n_dominants = signal_generator.dominant_sig.size();
        auto n_signals = signal_generator.n_signals;
        for (int peak_index = 0; peak_index < MAX_NUM_PEAKS; peak_index++) {
            for (auto dominant : signal_generator.dominant_sig) {
                if (IsBetweenInclusive(fft.peak_frequencies[axis][peak_index],
                            (int)(dominant.second) - abs_error,
                            (int)(dominant.second) + abs_error)) {
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

};

const std::array<InitParamMultiSignalWithRes, 8> MultiSignalTestParams = {
    // 0
    {{{InitFFTParamType{          .sample_rate_hz = 24,   .n_axes    = 1,   .window_size = 24},
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
      true}
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
        check_axis(axis);
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