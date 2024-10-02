#ifndef SRC_PLATFORM_STM32_MATH_RFFT_HPP_
#define SRC_PLATFORM_STM32_MATH_RFFT_HPP_

#include "arm_math.h"
#include "arm_const_structs.h"
typedef q15_t real_t;
#define M_2PI           6.28318530717958647692


/*
The function specifies arm_rfft_instance_q15 from CMSIS-DSP library based on the window size.
@param window_size: The size of the input array.
@param hanning_window: Pointer to the Hanning window container.
@param in: used fo incompatability with ubuntu version
@param out: used fo incompatability with ubuntu version
@param N: The size of the Hanning window.
@return: The plan for the r2c transform.
*/
inline arm_rfft_instance_q15 init_rfft(real_t** hanning_window, real_t** in,
                                        real_t** out, uint16_t N) {
    arm_rfft_instance_q15 _rfft_q15;
    switch (N) {
        case 256:
            _rfft_q15.fftLenReal = 256;
            _rfft_q15.twidCoefRModifier = 32U;
            _rfft_q15.pCfft = &arm_cfft_sR_q15_len128;
            break;

        case 512:
            _rfft_q15.fftLenReal = 512;
            _rfft_q15.twidCoefRModifier = 16U;
            _rfft_q15.pCfft = &arm_cfft_sR_q15_len256;
            break;

        case 1024:
            _rfft_q15.fftLenReal = 1024;
            _rfft_q15.twidCoefRModifier = 8U;
            _rfft_q15.pCfft = &arm_cfft_sR_q15_len512;
            break;
        default:
            N = 256;
            _rfft_q15.fftLenReal = 256;
            _rfft_q15.twidCoefRModifier = 32U;
            _rfft_q15.pCfft = &arm_cfft_sR_q15_len128;
    }
    _rfft_q15.pTwiddleAReal = (real_t *) realCoefAQ15;
    _rfft_q15.pTwiddleBReal = (real_t *) realCoefBQ15;
    _rfft_q15.ifftFlagR = 0;
    _rfft_q15.bitReverseFlagR = 1;

    // *in = new real_t[N];
    // *out = new real_t[N * 2];
    // *hanning_window = new real_t[N];

    *in = (real_t*)calloc(N, sizeof(real_t));
    *out = (real_t*)calloc(N * 2, sizeof(real_t));
    *hanning_window = (real_t*)calloc(N, sizeof(real_t));
    for (int n = 0; n < N; n++) {
        const float hanning_value = 0.5f * (1.f - cos(M_2PI * n / (N - 1)));
        (*hanning_window)[n] = hanning_value;
        arm_float_to_q15(&hanning_value,  &(*hanning_window)[n], 1);
    }

    return _rfft_q15;
}

/*
The function is written based on CMSIS-DSP library.
@param in: The input array.
@param out: The output array.
@param hanning_window: The Hanning window.
@param N: The size of the Hanning window.
*/
inline void apply_hanning_window(real_t* in, real_t* out, real_t* hanning_window, int N) {
    arm_mult_q15(in, hanning_window, out, N);
}

/*
The function is written based on CMSIS-DSP library.
@param _rfft_q15: The plan for the r2c transform.
@param in: The input array.
@param out: The output array.
*/
inline void rfft_one_cycle(arm_rfft_instance_q15 _rfft_q15, real_t* in, real_t* out) {
    arm_rfft_q15(&_rfft_q15, in, out);
}

inline void convert_real_t_to_float(real_t* in, float* out, uint16_t N) {
    arm_q15_to_float(in, out, N);
}

inline void convert_float_to_real_t(float* in, real_t* out, uint16_t N) {
    arm_float_to_q15(in, out, N);
}

// FFT output buffer is ordered as follows:
    // [real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1]
template<typename T>
inline void get_real_imag_by_index(T* in, T* out, uint16_t N, int index) {
    (void)N;
    out[0] = in[2 * index];
    out[1] = in[2 * index + 1];
}

#endif  // SRC_PLATFORM_STM32_MATH_RFFT_HPP_
