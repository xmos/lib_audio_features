#ifndef _mel_filter_h_
#define _mel_filter_h_

#include <stdint.h>
#include <stddef.h>

void apply_compact_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    int32_t* compact_mel_filter,
    const size_t num_mels,
    const int32_t mel_max);

void apply_compressed_mel_filter(
    int32_t* filtered,
    int32_t* input_bins,
    const size_t num_bins,
    int32_t* compressed_mel_filter,
    const int16_t indicies[][2],
    const size_t num_mels);

#endif
