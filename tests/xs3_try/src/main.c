#include <xs1.h>
#include <stdio.h>
#include <limits.h>
#include <xcore/hwtimer.h>

#include "dsp_math.h"
#include "bfp_math.h"
#include <math.h>

// I used python gen_fft_table.py --max_fft_log2 9 --dit for xs3_fft_lut.*
#define FFT_SIZE 64
unsigned time_start, time_stop;
#define TIME_START(x) time_start = get_reference_time();
#define TIME_STOP(msg) time_stop = get_reference_time(); printf("%s: %u ticks\n", msg, time_stop - time_start);

#define BFP_FLOAT(mant, exp) ((float)mant * pow(2, exp))


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
        // float val = (float)mant * pow(2, exp);
        float val = BFP_FLOAT(mant, exp);
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

//We assume the input is both positive and normalised so that the mantissa as maximum precision
void bfp_ln_s32(bfp_s32_t *ln_out, bfp_s32_t *bfp_in, unsigned n){
    const unsigned ln_frac_bits = 24;

    exponent_t exp = bfp_in->exp;
    headroom_t hr = bfp_in->hr;
    int32_t* in_data_ptr = bfp_in->data;
    int32_t* ln_out_data_ptr = ln_out->data;

    //Using ln(m.2^exp) = ln(m) + exp.ln(2) we can calculate the ln value of q8_24 result = dsp_math_log(uq8_24 in);
    printf("normalised exp: %d\n", exp);
    //Next we need to denormalise the BFP values because the input and output from log is Q24.
    exp += ln_frac_bits;
    printf("denormalised exp: %d\n", exp);
    
    exponent_t exp_min = (exponent_t)((float)INT_MIN / log(2) / (1 << ln_frac_bits));
    printf("exp_min: %d\n", (int)exp_min);
    if(exp < exp_min){
        printf("WARNING EXP MIN EXCEEDED - NEED TO HANDLE\n");
    }

    const q8_24 log_adder = (q8_24)(((int64_t)exp * (int64_t)(log(2) * (1 << ln_frac_bits))));
    printf("log_adder: %d\n", (int)log_adder);
    for(int i=0; i<n; i++){
        uq8_24 uq8_24_in_data = in_data_ptr[i];
        ln_out_data_ptr[i] = dsp_math_log(uq8_24_in_data) + log_adder;
        printf("ln idx %u: %d\n", i, ln_out_data_ptr[i]);
    }
}

int main(void){
    printf("Hello\n");

    uq8_24 x = 100 * 1 << 24;
    TIME_START()
    q8_24 result = dsp_math_log(x);
    TIME_STOP("dsp_math_log")
    printf("%f\n", BFP_FLOAT(result, -24));

    printf("max dsp_math_log: %d\n", (int)dsp_math_log(INT_MAX));
    //THIS FAILS
    //printf("max dsp_math_log: %d\n", (int)dsp_math_log(INT_MAX + 1));


    const unsigned calc_hr = 1; //Bool

    TIME_START()
    int32_t hanning_window_data[FFT_SIZE];
    make_hanning(hanning_window_data, FFT_SIZE);
    TIME_STOP("make_hanning")

    TIME_START()
    bfp_s32_t hanning_window;
    bfp_s32_init(&hanning_window, hanning_window_data, -31 /*exp*/, FFT_SIZE, calc_hr);
    TIME_STOP("bfp_s32_init")
    print_int_array(hanning_window_data, 4);
    print_bfp_array(&hanning_window, 4);


    int32_t windowed_samples_data[FFT_SIZE];
    get_noise_array(windowed_samples_data, FFT_SIZE);
    bfp_s32_t input_samples;
    bfp_s32_init(&input_samples, windowed_samples_data, 0 /*exp*/, FFT_SIZE, calc_hr);

    bfp_s32_t windowed_samples;
    bfp_s32_init(&windowed_samples, windowed_samples_data, 0 /*exp*/, FFT_SIZE, calc_hr);

    TIME_START()
    bfp_s32_mul(&windowed_samples, &input_samples, &hanning_window);
    TIME_STOP("bfp_s32_mul")
    // print_bfp_array_val(&windowed_samples, FFT_SIZE);
    

    // Initialize time domain data with samples.
    int32_t buffer[FFT_SIZE] = {1, 2 ,3, 4, 5, 6, 7, 8};
    bfp_s32_t samples;
    bfp_s32_init(&samples, buffer, 0 /*exp*/, FFT_SIZE, calc_hr);

    int32_t buffer_scaled[FFT_SIZE] = {0};
    bfp_s32_t samples_scaled;
    bfp_s32_init(&samples_scaled, buffer_scaled, 0 /*exp*/, FFT_SIZE, calc_hr);

    int32_t buffer_mulled[FFT_SIZE] = {0};
    bfp_s32_t samples_mulled;
    bfp_s32_init(&samples_mulled, buffer_mulled, 0 /*exp*/, FFT_SIZE, calc_hr);


    TIME_START()
    float_s32_t scalar = {/*mant*/8, /*exp*/0};
    bfp_s32_scale(&samples_scaled, &samples, scalar);
    TIME_STOP("bfp_s32_scale")


    for(int i=0; i<4; i++){
        print_bfp(&samples, i);
    }

    for(int i=0; i<4; i++){
        print_bfp(&samples_scaled, i);
    }

    bfp_s32_mul(&samples_mulled, &samples_scaled, &samples);


    for(int i=0; i<4; i++){
        print_bfp(&samples_mulled, i);
    }

    // Perform the forward DFT
    TIME_START()
    bfp_complex_s32_t* spectrum_cplx = bfp_fft_forward_mono(&samples);
    TIME_STOP("bfp_fft_forward_mono")

    // `samples` should no longer be used.
    // Operate on frequency domain data using `spectrum`
    
    bfp_s32_t mag;
    int32_t buffer_mag[FFT_SIZE/2];
    bfp_s32_init(&mag, buffer_mag, 0 /*exp*/, FFT_SIZE/2, calc_hr);
    TIME_START()
    bfp_complex_s32_mag(&mag, spectrum_cplx);
    TIME_STOP("bfp_complex_s32_mag")

    print_cpx_bfp_array(spectrum_cplx, 4);
    print_bfp_array(&mag, 4);

    // Use `samples` again to use new time domain data. 

    bfp_s32_t log_mag;
    bfp_s32_init(&log_mag, buffer_mulled, 0 /*exp*/, FFT_SIZE/2, calc_hr);
    bfp_ln_s32(&log_mag, &mag, 4);
    print_bfp_array(&log_mag, 4);



    printf("Fin\n");
}