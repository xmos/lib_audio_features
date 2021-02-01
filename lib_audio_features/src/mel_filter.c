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
    const int32_t mel_max,
    const size_t headroom){

    unsigned mel_even_idx = 0;
    unsigned mel_odd_idx = 1;
    // int64_t mel_even_accum = 0;
    // int64_t mel_odd_accum = 0;
    int32_t mel_even_accum_32 = 0;
    int32_t mel_odd_accum_32 = 0;
    int odd_mel_active_flag = 0;

    // print_line(input_bins, num_bins);

    for(int i = 0; i < num_bins; i++){
        int32_t even_mel = compact_mel_filter[i];
        int32_t odd_mel = odd_mel_active_flag ? mel_max - even_mel : 0;
        // int32_t odd_mel = mel_max - even_mel;
        // printf("%d %d %d\n", i, even_mel, odd_mel);

        // mel_even_accum += ((int64_t)input_bins[i] * (int64_t)even_mel) >> headroom;
        mel_even_accum_32 += (((int64_t)input_bins[i] * (int64_t)even_mel)) >> (headroom + 31);
        // printf("mel_even_accum %lld\n", mel_even_accum);
        // mel_odd_accum += ((int64_t)input_bins[i] * (int64_t)odd_mel) >> headroom;
        mel_odd_accum_32 += (((int64_t)input_bins[i] * (int64_t)odd_mel)) >> (headroom + 31);
        // printf("mel_odd_accum %lld\n", mel_odd_accum);

        if ((even_mel == 0) && (i != 0)){
            // filtered[mel_even_idx] = mel_even_accum >> 31;
            filtered[mel_even_idx] = mel_even_accum_32;
            //print("even", mel_even_accum)
            // mel_even_accum = 0;
            mel_even_accum_32 = 0;            
            mel_even_idx += 2;
        }

        if (even_mel == mel_max){
            odd_mel_active_flag = 1;
            if (mel_even_idx > 0){
                // filtered[mel_odd_idx] = mel_odd_accum >> 31;
                filtered[mel_odd_idx] = mel_odd_accum_32;
                //print("odd", mel_odd_accum)
                // mel_odd_accum = 0;
                mel_odd_accum_32 = 0;
                mel_odd_idx += 2;
            }
        }
    }
}

#if 0
unsigned apply_compressed_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    compressed_mel_t* compressed_mel_filter,
    const size_t num_mels){

    xassert(num_mels == AUDIO_FEATURES_NUM_MELS && "num_mels passed does not match AUDIO_FEATURES_NUM_MELS");
    xassert(num_bins == AUDIO_FEATURES_NUM_BINS && "num_bins passed does not match AUDIO_FEATURES_NUM_BINS");

    unsigned mel_even_idx = 0;
    unsigned mel_odd_idx = 1;
    int32_t mel_even_accum = 0;
    int32_t mel_odd_accum = 0;
    int odd_mel_active_flag = 0;

    for(int m = 0; m < num_mels; m++){
        int32_t mel_accum = 0; 

        unsigned start_idx = compressed_mel_filter->start_finish_indicies[m].start;
        unsigned end_idx = compressed_mel_filter->start_finish_indicies[m].end;
        int32_t* filter_ptr = compressed_mel_filter->filter_bank_ptrs[m];

        unsigned mel_idx = 0;
        //TODO replace with XS3 MATH
        for(int i = start_idx; i < end_idx; i++){
            mel_accum += ((int64_t)input_bins[i] * (int64_t)filter_ptr[mel_idx]) >> (31 + AUDIO_FEATURES_MEL_HEADROOM_BITS);
            mel_idx++;
        }
    }
    return AUDIO_FEATURES_MEL_HEADROOM_BITS;
}
#endif