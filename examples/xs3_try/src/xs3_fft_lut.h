// Copyright (c) 2020, XMOS Ltd, All rights reserved
#pragma once
#include "xs3_math.h"


/** @brief Maximum FFT length (log2) that can be performed using decimation-in-time. */
#define XS3_MAX_DIT_FFT_LOG2 9

/** @brief Convenience macro to index into the decimation-in-time FFT look-up table. 

	This will return the address at which the coefficients for the final pass of the real DIT
	FFT algorithm begin. 

	@param N	The FFT length.
*/
#define XS3_DIT_REAL_FFT_LUT(N) &xs3_dit_fft_lut[(N)-8]

extern const complex_s32_t xs3_dit_fft_lut[508]; // 4064 bytes
