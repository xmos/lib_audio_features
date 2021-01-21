#include <stdio.h>
#include <limits.h>
#include <xcore/hwtimer.h>

#include "xs3_math.h"
// I used python gen_fft_table.py --max_fft_log2 9 --dit for xs3_fft_lut.*
#define FFT_SIZE 256

#include "vtb_utilities.h"
void vtb_fd_hanning(dsp_complex_t *, unsigned int);

void print_bfp(bfp_s32_t *bfp, unsigned idx){
    int32_t* data = bfp->data;
    exponent_t exp = bfp->exp;
    // int32_t mant = ch ? data[idx].ch_b : data[idx].ch_a;
    int32_t mant = data[idx];
    float val = (float)mant * pow(2, exp);
    printf("idx: %u, mant: %d, exp: %d, float: %f\n", idx, (int)mant, exp, val);
}

void print_int_array(int32_t *array, unsigned n){
    for(int i=0; i<n; i++){
        printf("%d\n", array[i]);
    }
}

void print_cpx_array(dsp_complex_t *array, unsigned n){
    for(int i=0; i<n; i++){
        printf("Re: %d\tIm: %d\n", array[n].re, array[n].im);
    }
}

void make_hanning(int32_t *window, unsigned size){
    for (int i = 0; i < size; i++) {
    double multiplier = 0.5 * (1 - cos(2*3.1415926536*i/(size-1)));
        window[i] = (int32_t)(INT_MAX * multiplier);
    }
}

int main(void){
    printf("Hello\n");

    const unsigned calc_hr = 1; //Bool

    dsp_complex_t cpx_array[FFT_SIZE];
    for(int i=0; i<FFT_SIZE; i++){
        cpx_array[i].re = INT_MAX/100;
        cpx_array[i].im = INT_MAX/2;
    }
    print_cpx_array(cpx_array, 10);
    uint32_t t10 = get_reference_time();
    vtb_fd_hanning(cpx_array, FFT_SIZE);
    uint32_t t11 = get_reference_time();
    print_cpx_array(cpx_array, 10);
    printf("%u\n", t11 - t10);

    int32_t hanning_window[FFT_SIZE];
    make_hanning(hanning_window, FFT_SIZE);
    print_int_array(hanning_window, FFT_SIZE);

    int32_t windowed_samples_data[FFT_SIZE];
    bfp_s32_t windowed_samples;
    bfp_init_vect_s32(&windowed_samples, windowed_samples_data, 0 /*exp*/, FFT_SIZE, calc_hr);

    bfp_init_vect_s32()

    // Initialize time domain data with samples.
    int32_t buffer[FFT_SIZE] = {1, 2 ,3, 4, 5, 6, 7, 8};
    bfp_s32_t samples;
    bfp_init_vect_s32(&samples, buffer, 0 /*exp*/, FFT_SIZE, calc_hr);

    int32_t buffer_scaled[FFT_SIZE] = {0};
    bfp_s32_t samples_scaled;
    bfp_init_vect_s32(&samples_scaled, buffer_scaled, 0 /*exp*/, FFT_SIZE, calc_hr);

    int32_t buffer_mulled[FFT_SIZE] = {0};
    bfp_s32_t samples_mulled;
    bfp_init_vect_s32(&samples_mulled, buffer_mulled, 0 /*exp*/, FFT_SIZE, calc_hr);


    uint32_t t0 = get_reference_time();
    bfp_scalar_mul_vect_s32(&samples_scaled, &samples, /*mant*/8, /*exp*/0);
    uint32_t t1 = get_reference_time();


    for(int i=0; i<8; i++){
        print_bfp(&samples, i);
    }

    for(int i=0; i<8; i++){
        print_bfp(&samples_scaled, i);
    }

    uint32_t t2 = get_reference_time();
    bfp_mul_vect_s32(&samples_mulled, &samples_scaled, &samples);
    uint32_t t3 = get_reference_time();


    for(int i=0; i<8; i++){
        print_bfp(&samples_mulled, i);
    }

    // Perform the forward DFT
    uint32_t t4 = get_reference_time();
    bfp_complex_s32_t* spectrum = bfp_fft_forward_mono(&samples);
    uint32_t t5 = get_reference_time();

    // `samples` should no longer be used.
    // Operate on frequency domain data using `spectrum`
    

    // Perform the inverse DFT to go back to time domain
    bfp_fft_inverse_mono(spectrum); // returns (bfp_s32_t*) which is the address of `samples`
    // Use `samples` again to use new time domain data. 

    printf("%u, %u, %u\n", t1 - t0, t3 - t2, t5 - t4);



    printf("Fin\n");
}