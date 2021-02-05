#include <stdio.h>
#include <stdlib.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>

#ifdef MEL_FILTER_H_FILE
#include MEL_FILTER_H_FILE
#include "mel_filter.h"
#endif

#ifdef LN_TEST_VECTOR_LENGTH
#include "ln.h"
#endif



int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Usage: xsim --args xe-file <infile> <outfile> <iterations>. Ensure correct stage defines set before compilation.\n");
    }
    xassert(argc == 4);
    char* infile_name = argv[1];
    char* outfile_name = argv[2];
    int iterations = atoi(argv[3]);

    printf("Running: %s, infile: %s, outfile: %s, iterations: %d\n",
            argv[0], infile_name, outfile_name, iterations);

    FILE* infile = fopen(infile_name, "rb");
    xassert(infile);
    FILE* outfile = fopen(outfile_name, "wb");
    xassert(outfile);

    #ifdef MEL_FILTER_H_FILE
    for(int i = 0; i < iterations; i++){
        int32_t input_bins[AUDIO_FEATURES_NUM_BINS];
        int32_t output_mels[AUDIO_FEATURES_NUM_MELS];
        xassert(AUDIO_FEATURES_NUM_BINS == fread((void*)input_bins, sizeof(input_bins[0]), AUDIO_FEATURES_NUM_BINS, infile));
        uint32_t t0 = get_reference_time();
        #ifdef AUDIO_FEATURES_MEL_INDICIES_NAME
        apply_compressed_mel_filter(output_mels,
                                    input_bins,
                                    AUDIO_FEATURES_NUM_BINS,
                                    AUDIO_FEATURES_MEL_ARRAY_NAME,
                                    AUDIO_FEATURES_MEL_INDICIES_NAME,
                                    AUDIO_FEATURES_NUM_MELS);
        char type_string[] = "compressed";
        #else
        apply_compact_mel_filter(output_mels,
                                input_bins,
                                AUDIO_FEATURES_NUM_BINS,
                                AUDIO_FEATURES_MEL_ARRAY_NAME,
                                AUDIO_FEATURES_NUM_MELS,
                                AUDIO_FEATURES_MEL_MAX);
        char type_string[] = "compact";
        #endif
        uint32_t t1 = get_reference_time();
        printf("\nMEL processing time for %i bins and %i mels using %s: %u\n",
            AUDIO_FEATURES_NUM_BINS,
            AUDIO_FEATURES_NUM_MELS,
            type_string,
            (int)(t1 - t0));
        xassert(AUDIO_FEATURES_NUM_MELS == fwrite((void*)output_mels, sizeof(output_mels[0]), AUDIO_FEATURES_NUM_MELS, outfile));
    }
    #endif

    #ifdef LN_TEST_VECTOR_LENGTH
    const size_t ln_vect_len = atoi(LN_TEST_VECTOR_LENGTH);
    for(int i = 0; i < iterations; i++){
        int calc_hr = 0;
        int32_t input_array[ln_vect_len];
        exponent_t exp_in;
        bfp_s32_t ln_in;
        xassert(1 == fread((void*)&exp_in, sizeof(exponent_t), 1, infile));
        xassert(ln_vect_len == fread((void*)input_array, sizeof(input_array[0]), ln_vect_len, infile));
        bfp_s32_init(&ln_in, input_array, exp_in, ln_vect_len, calc_hr);

        int32_t output_array[ln_vect_len];
        bfp_s32_t ln_out;
        bfp_s32_init(&ln_out, output_array, 0, ln_vect_len, calc_hr);


        uint32_t t0 = get_reference_time();
        bfp_ln_s32(&ln_out, &ln_in, ln_vect_len);
        // memcpy(&output_array, &input_array, sizeof(output_array));
        // memcpy(&exp_out, &exp_in, sizeof(exp_out));
        uint32_t t1 = get_reference_time();
        printf("\nln processing time for vector len %i: %u\n",
            ln_vect_len,
            (int)(t1 - t0));
        xassert(1 == fwrite((void*)&ln_out.exp, sizeof(exponent_t), 1, outfile));
        xassert(ln_vect_len == fwrite((void*)output_array, sizeof(output_array[0]), ln_vect_len, outfile));
    }
    #endif
}