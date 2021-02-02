from scipy.fftpack import dct
import numpy as np

def hz2mel(f_Hz):
    """

    :param f_Hz: frequency in Hz
    :return f_mel: frequency in mel

    """
    f_mel = 2595 * np.log10(1 + f_Hz/700.0)
    return f_mel

def mel2hz(f_mel):
    """

    :param f_mel: frequency in mel
    :return f_mel: frequency in Hz

    """
    f_Hz = 700 * (10**(f_mel/2595.0) - 1)
    return f_Hz

def mel_filterbank(Fs, nfft, n_filters=24, f_low=0, f_high=None, add_1=0):
    """
    :param Fs: sampling rate
    :param nfft: FFT length
    :param n_filters: number of filters dividing the range [f_low:f_high]
    :param f_low: lower freq [Hz] where the filters start
    :param f_high: higher freq [Hz] where the filters stop (usually Fs/2)
    :param add_1: 1: add two more filters in the beginning and end such that the sum over all filterbanks is 1; 0: don't add extra filters at the ends
    :return fbank: mel filterbank

    """
    f_high = f_high or Fs/2.0

    # sanity check
    assert (f_high <= Fs/2.0), "Mel filterbank higher frequency cannot exceed Fs/2!"

    # convert freq. from Hz range to mel
    mel_low = hz2mel(f_low)
    mel_high = hz2mel(f_high)

    if add_1 == 1:
        mel_bins = np.linspace(mel_low, mel_high, n_filters + 2)
        
        # determine freq. bins in Hz that correspond to the mel_bins
        Hz_bins = np.floor(mel2hz(mel_bins) * (nfft + 1) / float(Fs))

        # create triangular mel filterbank  of size [number of mel filters x number of fft bins up to Fs/2]
        fbank = np.zeros([n_filters+2,nfft//2+1]) # add 2 more filters at the beginning and end such that they all add up to 1

        # first filter
        for bdx in range(int(Hz_bins[0]), int(Hz_bins[1])):
            fbank[0, bdx] = float(Hz_bins[1] - bdx) / float(Hz_bins[1] - Hz_bins[0])
            
        # 2nd to last-1 filters
        for fdx in range(0,n_filters):
            for bdx in range(int(Hz_bins[fdx]), int(Hz_bins[fdx+1])): # left side of triangular filter
                fbank[fdx+1,bdx] = float(bdx - Hz_bins[fdx]) / float(Hz_bins[fdx+1] - Hz_bins[fdx])
            for bdx in range(int(Hz_bins[fdx+1]), int(Hz_bins[fdx+2])):  # right side of triangular filter
                fbank[fdx+1, bdx] = float(Hz_bins[fdx+2] - bdx) / float(Hz_bins[fdx+2] - Hz_bins[fdx+1])
                
        # last filter
        rbdx = int(Hz_bins[n_filters+1])
        for bdx in range(int(Hz_bins[n_filters]), int(Hz_bins[n_filters+1])+1):
            fbank[n_filters+1, bdx] = float(Hz_bins[n_filters+1]-rbdx) / float(Hz_bins[n_filters+1] - Hz_bins[n_filters])
            rbdx-=1
    else:
        mel_bins = np.linspace(mel_low, mel_high, n_filters+2)
        
        # determine freq. bins in Hz that correspond to the mel_bins
        Hz_bins = np.floor(mel2hz(mel_bins) * (nfft + 1) / float(Fs))

        # create triangular mel filterbank  of size [number of mel filters x number of fft bins up to Fs/2]
        fbank = np.zeros([n_filters, nfft // 2 + 1])  # add 2 more filters at the beginning and end such that they all add up to 1

        # 1st to last filters
        fbank_Hz_range = np.empty((n_filters, 2))  # start and end in [Hz] of filters in FB
        for fdx in range(0, n_filters):
            for bdx in range(int(Hz_bins[fdx]), int(Hz_bins[fdx + 1])):  # left side of triangular filter
                fbank[fdx, bdx] = float(bdx - Hz_bins[fdx]) / float(Hz_bins[fdx + 1] - Hz_bins[fdx])
            for bdx in range(int(Hz_bins[fdx + 1]), int(Hz_bins[fdx + 2])):  # right side of triangular filter
                fbank[fdx, bdx] = float(Hz_bins[fdx + 2] - bdx) / float(Hz_bins[fdx + 2] - Hz_bins[fdx + 1])
            fbank_Hz_range[fdx, :] = (np.array([Hz_bins[fdx], Hz_bins[fdx+2]])*float(Fs)/(nfft + 1)).astype(int)

    return fbank