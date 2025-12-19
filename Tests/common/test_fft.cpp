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

struct InitMultiSignalsParamType   {
    uint16_t sample_rate_hz;
    size_t n_signals;
    uint16_t max_freq;
    uint16_t min_freq;
    float abs_error;
};

std::random_device rd;
class MultiSignalsSinGenerator {
public:
    uint16_t max_freq = 0;
    size_t n_signals = 0;
    uint16_t min_freq = 0;
    uint16_t sample_rate_hz = 0;
    std::vector<SinSignalGenerator> signals_generator;
    std::vector<std::pair<float, float>> dominant_sig;
    float abs_error;

    void init() {
        signals_generator.resize(n_signals);
        for (size_t j = 0; j < n_signals; j++) {
            std::uniform_int_distribution<uint16_t> freq_dist(min_freq, max_freq);
            std::uniform_int_distribution<uint16_t> amp_dist(1, 100);
            uint16_t freq_hz = 0;
            // generate freq_hz until it is not close to any other freq_hz,
            // otherwise the estimated frequency will be doubled, no way to avoid that
            while (true) {
                bool is_close = false;
                freq_hz = freq_dist(rd);
                for (size_t i = 0; i < j; i++) {
                    if (abs(signals_generator[i].freq_hz - freq_hz) < abs_error) {
                        is_close = true;
                        break;
                    }
                }
                if (!is_close) break;
            }
            uint16_t amplitude = amp_dist(rd);

            signals_generator[j] = SinSignalGenerator(sample_rate_hz, freq_hz, amplitude);
        }
        std::stable_sort(signals_generator.begin(), signals_generator.end(),
            [](const SinSignalGenerator& a, const SinSignalGenerator& b) {
                return a.amplitude > b.amplitude;
                });
        auto domimant_size = std::min(int(n_signals), 3);
        dominant_sig.resize(domimant_size);
        for (size_t j = 0; j < domimant_size; j++) {
            dominant_sig[j] = std::make_pair(signals_generator[j].amplitude,
                                                signals_generator[j].freq_hz);
        }
    }

    MultiSignalsSinGenerator() = default;

    MultiSignalsSinGenerator
        (size_t n_sig, uint16_t sample_rate, uint16_t max_frequency, uint16_t min_frequency) :
                                                max_freq(max_frequency),
                                                min_freq(min_frequency),
                                                n_signals(n_sig),
                                                sample_rate_hz(sample_rate) {
        init();
    }

    explicit MultiSignalsSinGenerator(InitMultiSignalsParamType parameters) :
                                                max_freq(parameters.max_freq),
                                                min_freq(parameters.min_freq),
                                                n_signals(parameters.n_signals),
                                                sample_rate_hz(parameters.sample_rate_hz),
                                                abs_error(parameters.abs_error) {
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
    bool result;
    std::vector<T> signals_generator;
    InitFFTParamType fft_parameters;
    std::vector<InitParamType> signals_parameters;
    float abs_error;
    float error_margin;

    void print_fft_parameters() {
        std::cout << "FFT Parameters: " << std::endl;
        std::cout << "  Sample Rate (Hz): " << fft_parameters.sample_rate_hz << std::endl;
        std::cout << "  Window Size: " << fft_parameters.window_size << std::endl;
        std::cout << "  Number of Axes: " << static_cast<int>(fft_parameters.n_axes) << std::endl;
    }

    virtual bool init() {
        bool ret = fft.init(fft_parameters.window_size, fft_parameters.n_axes,
                                                        fft_parameters.sample_rate_hz);
        abs_error = error_margin * fft._resolution_hz;
        if (ret == false) return false;
        signals_generator.resize(fft_parameters.n_axes);
        return true;
    }
};

class TestFFTOneSignalParametrized : public TestFFTBase<SinSignalGenerator,
                                            InitOneSignParamType>,
                        public testing::WithParamInterface<InitParamOneSignalWithRes> {
public:
    bool is_heat_all_peaks{true};


    bool init() override {
        if (!super::init()) {
            return false;
        }

        for (int i = 0; i < fft_parameters.n_axes; i++) {
            auto signal_parameters = signals_parameters[i];
            signals_generator[i] = SinSignalGenerator(signal_parameters);
        }
        return true;
    }

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
            if (fft.peak_snr[axis][j] == 0 || fft.peak_frequencies[axis][j] == 0) {
                continue;
            }
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
        error_margin = 1.5f;
        InitParamType<InitOneSignParamType> init_parameters = GetParam().parameters;
        result = GetParam().result;
        fft_parameters = init_parameters.fft_parameters;
        // expand signal_parameters to multiple axes if needed
        signals_parameters.resize(fft_parameters.n_axes);
        for (int i = 0; i < fft_parameters.n_axes; i++) {
            signals_parameters[i] = init_parameters.signals_parameters;
        }
        // To print FFT parameters for debug uncomment the following line
        // print_signal_parameters();
        // print_fft_parameters();
    }
 protected:
    typedef TestFFTBase<SinSignalGenerator, InitOneSignParamType> super;
};

const std::array<InitParamOneSignalWithRes, 10> OneSignalTestParams = {
{
    // 0
    {{InitFFTParamType{      .sample_rate_hz = 100,  .n_axes   = 1,   .window_size  = 50},
      InitOneSignParamType{  .sample_rate_hz = 100,  .freq_hz  = 4,   .amplitude    = 100}},
      true},
    // 1
    {{InitFFTParamType{      .sample_rate_hz = 512,  .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 512,  .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 2
    {{InitFFTParamType{      .sample_rate_hz = 1000, .n_axes   = 1,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1000, .freq_hz  = 100, .amplitude    = 10}},
      true},
    // 3
    {{InitFFTParamType{      .sample_rate_hz = 1024, .n_axes   = 3,   .window_size  = 512},
      InitOneSignParamType{  .sample_rate_hz = 1024, .freq_hz  = 100, .amplitude    = 100}},
      true},
    // 4 Too big frequency
    {{InitFFTParamType{      .sample_rate_hz = 256,  .n_axes   = 3,   .window_size  = 256},
      InitOneSignParamType{  .sample_rate_hz = 256,  .freq_hz  = 200, .amplitude    = 1}},
      false},
    // 5 Too big window size for sample rate
    {{InitFFTParamType{     .sample_rate_hz  = 100,   .n_axes   = 1,  .window_size  = 200},
      InitOneSignParamType{ .sample_rate_hz  = 100,  .freq_hz  = 100, .amplitude    = 1}},
      false},
    // 6 Unmatched sample rates
    {{InitFFTParamType{     .sample_rate_hz  = 10,   .n_axes   = 1,  .window_size  = 200},
      InitOneSignParamType{ .sample_rate_hz  = 200,  .freq_hz  = 100, .amplitude    = 1}},
      false},
    // 7 FFT resolution is too low
    {{InitFFTParamType{     .sample_rate_hz  = 100,   .n_axes   = 1,  .window_size  = 2},
      InitOneSignParamType{ .sample_rate_hz  = 100,  .freq_hz   = 20, .amplitude    = 10}},
      false},
    // 8 Window size is zero
    {{InitFFTParamType{     .sample_rate_hz  = 100,   .n_axes   = 1,  .window_size  = 0},
      InitOneSignParamType{ .sample_rate_hz  = 100,  .freq_hz   = 20, .amplitude    = 10}},
      false},
}
};

TEST_P(TestFFTOneSignalParametrized, CheckOnWindow) {
    if (!init()) {
        EXPECT_EQ(result, false);
        return;
    }
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
    if (!init()) {
        EXPECT_EQ(result, false);
        return;
    }
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
        error_margin = 10.0f;
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
    }
    bool init() override {
        if (!super::init()) {
            return false;
        }

        for (int i = 0; i < fft_parameters.n_axes; i++) {
            auto signal_parameters = signals_parameters[i];
            signal_parameters.abs_error = error_margin * fft._resolution_hz;
            signals_generator[i] = MultiSignalsSinGenerator(signal_parameters);
        }
        return true;
    }
    void check_axis(int axis) {
        bool heat_peak {false};
        auto signal_generator = signals_generator[axis];
        auto n_dominants = signal_generator.dominant_sig.size();
        auto n_signals = signal_generator.n_signals;
        for (int peak_index = 0; peak_index < MAX_NUM_PEAKS; peak_index++) {
            for (auto dominant : signal_generator.dominant_sig) {
                if (fft.peak_snr[axis][peak_index] == 0.0f) {
                    continue;
                }
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

 protected:
    typedef TestFFTBase<MultiSignalsSinGenerator, InitMultiSignalsParamType> super;
};

const std::array<InitParamMultiSignalWithRes, 6> MultiSignalTestParams = {
    {
    // Use a smaller frequency range to avoid edge cases when randomizing
    // 0
    {{InitFFTParamType{          .sample_rate_hz = 256, .n_axes     = 1,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 256, .n_signals  = 2,   .max_freq    = 100,
                                                                           .min_freq    = 5}},
      true},
    // 1
    {{InitFFTParamType{          .sample_rate_hz = 256, .n_axes     = 1,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 256, .n_signals  = 5,   .max_freq    = 100,
                                                                           .min_freq    = 5}},
      true},
    // 2
    {{InitFFTParamType{          .sample_rate_hz = 256, .n_axes     = 2,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 256, .n_signals  = 2,   .max_freq    = 100,
                                                                           .min_freq    = 5}},
      true},
    // 3
    {{InitFFTParamType{          .sample_rate_hz = 256, .n_axes     = 2,   .window_size = 256},
      InitMultiSignalsParamType{ .sample_rate_hz = 256, .n_signals  = 5,   .max_freq    = 100,
                                                                           .min_freq    = 5}},
      true},
    // 4
    {{InitFFTParamType{          .sample_rate_hz = 512,  .n_axes    = 1,   .window_size = 512},
      InitMultiSignalsParamType{ .sample_rate_hz = 512,  .n_signals = 5,   .max_freq    = 200,
                                                                           .min_freq    = 5}},
      true},
    // 5 Too big frequency
    {{InitFFTParamType{          .sample_rate_hz = 512, .n_axes     = 3,   .window_size = 512},
      InitMultiSignalsParamType{ .sample_rate_hz = 512, .n_signals  = 10,  .max_freq    = 1000,
                                                                           .min_freq    = 512}},
      false},
    }
};

TEST_P(TestFFTOnMultiSignalsParametrized, CheckOnWindow) {
    if (!init()) {
        EXPECT_EQ(result, false);
        return;
    }
    for (int i = 0; i < fft_parameters.window_size + 1; i++) {
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
