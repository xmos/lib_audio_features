#ifndef _mel_filter_h_
#define _mel_filter_h_

#include <stdint.h>
#include <stddef.h>

// typedef struct {
//     unsigned start;
//     unsigned end;
// } start_finish_indicies_t;

// typedef struct {
//     start_finish_indicies_t start_finish_indicies[AUDIO_FEATURES_NUM_MELS];
//     int32_t *filter_bank_ptrs[AUDIO_FEATURES_NUM_MELS];
// } compressed_mel_t;


void apply_compact_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    int32_t* compact_mel_filter,
    const size_t num_mels,
    const int32_t mel_max,
    const size_t headroom);

#endif
