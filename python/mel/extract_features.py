from scipy.fftpack import dct
import numpy as np

MEL_MAX = 16777216

def compute_mfcc(
    spec,
    melFilter,
    pow=1,
):
    """
    Function takes a 2D magnitude spectrogram and a mel-FB as input and computes the log-mels and mfccs.
    Parameters:
        spec: x magnitude spectrogram [int(frame_length / 2) + 1 x nframes]
        melFilter: 2D matrix of mel filterbank [n_mels x nframes]
        pow: scalar power of log-mels before after log operation and before DCT (default is 1)
    Returns:
        log_mels: log mel spectrogram [nframes x n_mel_coefs]
        mfccs: MFCC matrix (output of applying a DCT on log-mels) [nframes x n_mel_coefs]
    """

    # compute log-mel spectrum^pow
    log_mels = np.log(np.matmul(melFilter, spec) / MEL_MAX).T ** pow

    # compute DCT
    mfccs = dct(log_mels)  # remove first filter and apply DCT

    return log_mels, mfccs
