#include <stdio.h>

#include "xs3_math.h"

#define N 512

int main(void){
    printf("Hello\n");


    // Initialize time domain data with samples.
    int32_t buffer[N] = {0};
    bfp_s32_t samples;
    bfp_init_vect_s32(&samples, buffer, 0, N, 1);
    // Perform the forward DFT
    {
        bfp_complex_s32_t* spectrum = bfp_fft_forward_mono(&samples);
        // `samples` should no longer be used.
        // Operate on frequency domain data using `spectrum`
        

        // Perform the inverse DFT to go back to time domain
        bfp_fft_inverse_mono(spectrum); // returns (bfp_s32_t*) which is the address of `samples`
    }
    // Use `samples` again to use new time domain data. 

    printf("Fin\n");
}