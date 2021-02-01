#include <stdio.h>
#include <stdlib.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>

#include MEL_FILTER_H_FILE
#include "mel_filter.h"

enum feature_stage{
    MEL
};



int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Usage: xsim --args xe-file <test_stage> <infile> <outfile> <iterations>\n");
    }
    xassert(argc == 5);
    int stage = atoi(argv[1]);
    char* infile_name = argv[2];
    char* outfile_name = argv[3];
    int iterations = atoi(argv[4]);

    printf("Running: %s, stage: %d, infile: %s, outfile: %s, iterations: %d\n",
            argv[0], stage, infile_name, outfile_name, iterations);

    FILE* infile = fopen(infile_name, "rb");
    xassert(infile);
    FILE* outfile = fopen(outfile_name, "wb");
    xassert(outfile);

    switch(stage){
        case MEL:
            printf("MEL test\n");

            for(int i = 0; i < iterations; i++){
                int32_t input_bins[AUDIO_FEATURES_NUM_BINS];
                int32_t output_mels[AUDIO_FEATURES_NUM_MELS];
                xassert(AUDIO_FEATURES_NUM_BINS == fread((void*)input_bins, sizeof(input_bins[0]), AUDIO_FEATURES_NUM_BINS, infile));
                uint32_t t0 = get_reference_time();
                apply_compact_mel_filter(output_mels,
                                        input_bins,
                                        AUDIO_FEATURES_NUM_BINS,
                                        AUDIO_FEATURES_MEL_ARRAY_NAME,
                                        AUDIO_FEATURES_NUM_MELS,
                                        AUDIO_FEATURES_MEL_MAX);
                uint32_t t1 = get_reference_time();
                printf("MEL time: %lu\n", t1 - t0);
                xassert(AUDIO_FEATURES_NUM_MELS == fwrite((void*)output_mels, sizeof(output_mels[0]), AUDIO_FEATURES_NUM_MELS, outfile));
            }
        break;
        default:
            printf("Invalid stage: %d\n", stage);
            xassert(0);
        break;
    }
}