#include <xs1.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <xcore/hwtimer.h>

#include "bfp_math.h"
#include "mfcc.h"

// I used python gen_fft_table.py --max_fft_log2 9 --dit for xs3_fft_lut.*
#define FFT_SIZE 512
unsigned time_start, time_stop;
#define TIME_START(x) time_start = get_reference_time();
#define TIME_STOP(msg) time_stop = get_reference_time(); printf("%s: %u ticks\n", msg, time_stop - time_start);

#define BFP_FLOAT(mant, exp) ((double)mant * pow(2, exp))


void print_bfp(bfp_s32_t *bfp, unsigned idx){
    int32_t* data = bfp->data;
    exponent_t exp = bfp->exp;
    // int32_t mant = ch ? data[idx].ch_b : data[idx].ch_a;
    int32_t mant = data[idx];
    float val = (float)mant * pow(2, exp);
    printf("idx: %u, mant: %d, exp: %d, float: %f\n", (unsigned)idx, (int)mant, exp, val);
}

void print_bfp_array(bfp_s32_t *bfp, unsigned n){
    for(int i=0; i<n; i++){
        print_bfp(bfp, i);
    }
}

void print_bfp_array_val(bfp_s32_t *bfp, unsigned n){
    for(int i=0; i<n; i++){
        int32_t* data = bfp->data;
        exponent_t exp = bfp->exp;
        int32_t mant = data[i];
        // float val = (double)mant * pow(2, exp);
        double val = BFP_FLOAT(mant, exp);
        printf("%f\n", val);
    }
}

void print_int_array(int32_t *array, unsigned n){
    for(int i=0; i<n; i++){
        printf("%d\n", (int)array[i]);
    }
}

void print_cpx_bfp(bfp_complex_s32_t *bfp, unsigned idx){
    printf("Re: %f\tIm: %f\n", BFP_FLOAT(bfp->data[idx].re, bfp->exp), BFP_FLOAT(bfp->data[idx].im, bfp->exp));
}

void print_cpx_bfp_array(bfp_complex_s32_t *bfp, unsigned n){
    for(int i=0; i<n; i++){
        print_cpx_bfp(bfp, i);
    }
}

void make_hanning(int32_t *window, unsigned size){
    for (int i = 0; i < size; i++) {
    double multiplier = 0.5 * (1 - cos(2*3.1415926536*i/(size-1)));
        window[i] = (int32_t)(INT_MAX * multiplier);
    }
}

void get_noise_array(int32_t *array, unsigned n);



int main(void){
    printf("Hello\n");


    TIME_START()
    int32_t hanning_window_data[FFT_SIZE];
    make_hanning(hanning_window_data, FFT_SIZE);
    TIME_STOP("make_hanning")

    #define MFCC_SIZE 20
    audio_features_mfcc_t mfcc_config;
    init_mfcc(&mfcc_config, MFCC_SIZE, MFCC_SIZE, 9, hanning_window_data);

    int32_t samps[FFT_SIZE];
    get_noise_array(samps, FFT_SIZE);

    int32_t mfccs[MFCC_SIZE];

    mfcc(mfccs, samps, &mfcc_config);



    printf("Fin\n");
}