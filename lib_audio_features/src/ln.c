#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "ln.h"

//We assume the input is both positive and normalised so that the mantissa has maximum precision
void bfp_ln_s32(bfp_s32_t *ln_out, bfp_s32_t *bfp_in, unsigned n){
    const unsigned ln_frac_bits = 24;
    int apply_epsilon = 0; //For when input value approaches zero, we have to handle specially
    int apply_clip = 0; //For when the input exponent permits a number greater than can be handled

    exponent_t exp_in = bfp_in->exp;
    headroom_t hr = bfp_in->hr;
    int32_t* in_data_ptr = bfp_in->data;
    int32_t* ln_out_data_ptr = ln_out->data;

    //Using ln(m.2^exp) = ln(m) + exp.ln(2) we can calculate the ln value of q8_24 result = dsp_math_log(uq8_24 in);
    // printf("normalised exp: %d\n", exp_in);
    //Next we need to denormalise the BFP values because the input and output from log is Q24.
    exp_in += ln_frac_bits;
    // printf("denormalised exp: %d\n", exp_in);
    
    exponent_t exp_min = (exponent_t)((double)INT_MIN / log(2) / (1 << ln_frac_bits)); //-184
    exponent_t exp_max = (exponent_t)((double)INT_MAX / log(2) / (1 << ln_frac_bits)) - 31; //154
    int32_t clip_val = (int32_t)(log(2 * pow(2.0, exp_max)) * (1 << ln_frac_bits));//106.7446658062 << 24

    // printf("exp_min: %d\n", (int)exp_min);
    // printf("exp_max: %d\n", (int)exp_max);
    // printf("clip_val: %d\n", (int)clip_val);

    if(exp_in < exp_min){
        printf("WARNING EXP MIN EXCEEDED\n");
        apply_epsilon = 1;
    }
    if(exp_in > exp_max){
        printf("WARNING EXP MAX EXCEEDED\n");
        apply_clip = 1;
    }

    const q8_24 log_adder = (q8_24)(((int64_t)exp_in * (int64_t)(log(2) * (1 << ln_frac_bits))));
    // printf("log_adder: %d\n", (int)log_adder);
    for(int i=0; i<n; i++){
        uq8_24 uq8_24_in_data = in_data_ptr[i];
        if(apply_epsilon || uq8_24_in_data == 0){
            ln_out_data_ptr[i] = INT_MIN;
        }else if(apply_clip){
            ln_out_data_ptr[i] = clip_val;

        }else{
            ln_out_data_ptr[i] = dsp_math_log(uq8_24_in_data) + log_adder;
        // printf("ln idx %u mantissa: %d\n", i, (int)ln_out_data_ptr[i]);
        }
    }
    //We don't need to normalise since the output range of ln is -128 to +127 which covers
    //2.57E-56 to 3.888E+55 with clipping protection, which should be enough and exceeds SPFP input range.
    //log does a good job of squishfying the large dynamic range of the input..
    ln_out->exp = -ln_frac_bits;
}