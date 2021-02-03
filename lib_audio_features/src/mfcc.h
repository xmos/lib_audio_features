#include <stdio.h>
#include <limits.h>

typedef enum{
    AUDIO_FEATURES_WINDOW_HANNING
} audio_features_window_t;

typedef struct {
    unsigned num_cep;
    unsigned num_filt;
    unsigned nfft_log2;
    int32_t* window;
} audio_features_mfcc_t;

void init_mfcc(audio_features_mfcc_t * config,
    unsigned num_cep,
    unsigned num_filt,
    unsigned nfft_log2,
    int32_t* window);

void mfcc(int32_t* mfccs, int32_t* samps, audio_features_mfcc_t * config);