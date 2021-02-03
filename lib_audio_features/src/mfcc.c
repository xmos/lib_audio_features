#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>

#include "mfcc.h"
#include "bfp_math.h"
#include "ln.h"
#include "dsp_dct.h"

void init_mfcc(audio_features_mfcc_t * config,
    unsigned num_cep,
    unsigned num_filt,
    unsigned nfft_log2,
    int32_t* window){

    config->num_cep = num_cep;
    config->num_filt = num_filt;
    config->nfft_log2 = nfft_log2;
    config->window = window;
}

void print_cpx_bfp(bfp_complex_s32_t *bfp, unsigned idx);
void print_cpx_bfp_array(bfp_complex_s32_t *bfp, unsigned n);
void print_bfp_array(bfp_s32_t *bfp, unsigned n);

#define PRINT 1
unsigned time_start_mfcc, time_stop_mfcc;
#define TIME_START(x) time_start_mfcc = get_reference_time();
#if PRINT
#define TIME_STOP(msg) time_stop_mfcc = get_reference_time(); printf("%s: %u ticks\n", msg, time_stop_mfcc - time_start_mfcc);
#else
#define TIME_STOP(msg)
#endif

void mfcc(int32_t* mfccs, int32_t* samps, audio_features_mfcc_t *config){
    const unsigned fft_size = 1 << config->nfft_log2;
    printf("fft_size: %d\n", fft_size);


    const unsigned calc_hr = 1; //Bool

    TIME_START()
    bfp_s32_t window;
    bfp_s32_init(&window, config->window, -31 /*exp*/, fft_size, calc_hr);
    TIME_STOP("bfp_s32_init")
    print_bfp_array(&window, 4);


    bfp_s32_t input_samples;
    bfp_s32_init(&input_samples, samps, 0 /*exp*/, fft_size, calc_hr);

    bfp_s32_t windowed_samples;
    bfp_s32_init(&windowed_samples, samps, 0 /*exp*/, fft_size, calc_hr);

    TIME_START()
    bfp_s32_mul(&windowed_samples, &input_samples, &window);
    TIME_STOP("bfp_s32_mul")
    // print_bfp_array_val(&windowed_samples, FFT_SIZE);
    
    // Perform the forward DFT
    TIME_START()
    bfp_complex_s32_t* spectrum_cplx = bfp_fft_forward_mono(&windowed_samples);
    TIME_STOP("bfp_fft_forward_mono")

    
    bfp_s32_t mag;
    bfp_s32_init(&mag, samps, 0 /*exp*/, fft_size/2, calc_hr);
    TIME_START()
    bfp_complex_s32_mag(&mag, spectrum_cplx);
    TIME_STOP("bfp_complex_s32_mag")

    print_cpx_bfp_array(spectrum_cplx, 10);
    print_bfp_array(&mag, 10);


    bfp_s32_t log_mag;
    bfp_s32_init(&log_mag, samps, 0 /*exp*/, fft_size/2, calc_hr);
    TIME_START()
    bfp_ln_s32(&log_mag, &mag, fft_size/2);
    TIME_STOP("bfp_ln_s32")
    print_bfp_array(&log_mag, 10);


    //Now do DCT

    xassert(config->num_cep <= 48);
    dsp_dct_forward48(mfccs, samps);
}