#include <limits.h>
#include "mel_filter.h"
#include <stdio.h>


void print_line(int32_t *data, unsigned n){
    for(int i = 0; i < n; i++){
        printf("%ld, ", data[i]);
    }
    printf("\n");
}


void apply_compact_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    int32_t* compact_mel_filter,
    const size_t num_mels,
    const int32_t mel_max){

    unsigned mel_even_idx = 0;
    unsigned mel_odd_idx = 1;
    int64_t mel_even_accum = 0;
    int64_t mel_odd_accum = 0;
    // int32_t mel_even_accum_32 = 0;
    // int32_t mel_odd_accum_32 = 0;
    int odd_mel_active_flag = 0;

    // print_line(input_bins, num_bins);

    for(int i = 0; i < num_bins; i++){
        int32_t even_mel = compact_mel_filter[i];
        int32_t odd_mel = odd_mel_active_flag ? mel_max - even_mel : 0;
        // printf("%d %d %d\n", i, even_mel, odd_mel);

        mel_even_accum += ((int64_t)input_bins[i] * (int64_t)even_mel);
        // mel_even_accum_32 += (((int64_t)input_bins[i] * (int64_t)even_mel)) >> (31);
        // printf("mel_even_accum %lld\n", mel_even_accum);
        mel_odd_accum += ((int64_t)input_bins[i] * (int64_t)odd_mel);
        // mel_odd_accum_32 += (((int64_t)input_bins[i] * (int64_t)odd_mel)) >> (31);
        // printf("mel_odd_accum %lld\n", mel_odd_accum);

        if ((even_mel == 0) && (i != 0)){
            filtered[mel_even_idx] = mel_even_accum >> 31;
            // filtered[mel_even_idx] = mel_even_accum_32;
            //print("even", mel_even_accum)
            mel_even_accum = 0;
            // mel_even_accum_32 = 0;            
            mel_even_idx += 2;
        }

        if (even_mel == mel_max){
            odd_mel_active_flag = 1;
            if (mel_even_idx > 0){
                filtered[mel_odd_idx] = mel_odd_accum >> 31;
                // filtered[mel_odd_idx] = mel_odd_accum_32;
                //print("odd", mel_odd_accum)
                mel_odd_accum = 0;
                // mel_odd_accum_32 = 0;
                mel_odd_idx += 2;
            }
        }
    }
}

void apply_compressed_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    int32_t* compressed_mel_filter,
    const int16_t indicies[][2],
    const size_t num_mels){

    print_line(input_bins, num_bins);

    unsigned filter_idx = 0;
    for(int m = 0; m < num_mels; m++){

        unsigned start_idx = indicies[m][0];
        unsigned end_idx = indicies[m][1];

        //TODO replace with XS3 MATH
        int32_t mel_accum = 0; 
        for(int idx = start_idx; idx < end_idx; idx++){
            mel_accum += ((int64_t)input_bins[idx] * (int64_t)compressed_mel_filter[filter_idx]) >> (31);
            filter_idx++;
        }
        filtered[m] = mel_accum;
    }
}
