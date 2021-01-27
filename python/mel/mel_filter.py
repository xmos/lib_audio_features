import numpy
import sys

def hz2mel(hz):
    """Convert a value in Hertz to Mels
    :param hz: a value in Hz. This can also be a numpy array, conversion proceeds element-wise.
    :returns: a value in Mels. If an array was passed in, an identical sized array is returned.
    """
    return 2595 * numpy.log10(1+hz/700.)

def mel2hz(mel):
    """Convert a value in Mels to Hertz
    :param mel: a value in Mels. This can also be a numpy array, conversion proceeds element-wise.
    :returns: a value in Hertz. If an array was passed in, an identical sized array is returned.
    """
    return 700*(10**(mel/2595.0)-1)

def get_filterbanks(nfilt=20,nfft=512,samplerate=16000,lowfreq=0,highfreq=None):
    """Compute a Mel-filterbank. The filters are stored in the rows, the columns correspond
    to fft bins. The filters are returned as an array of size nfilt * (nfft/2 + 1)
    :param nfilt: the number of filters in the filterbank, default 20.
    :param nfft: the FFT size. Default is 512.
    :param samplerate: the sample rate of the signal we are working with, in Hz. Affects mel spacing.
    :param lowfreq: lowest band edge of mel filters, default 0 Hz
    :param highfreq: highest band edge of mel filters, default samplerate/2
    :returns: A numpy array of size nfilt * (nfft/2 + 1) containing filterbank. Each row holds 1 filter.
    """
    highfreq= highfreq or samplerate/2
    assert highfreq <= samplerate/2, "highfreq is greater than samplerate/2"

    # compute points evenly spaced in mels
    lowmel = hz2mel(lowfreq)
    highmel = hz2mel(highfreq)
    melpoints = numpy.linspace(lowmel,highmel,nfilt+2)
    # our points are in Hz, but we use fft bins, so we have to convert
    #  from Hz to fft bin number
    bin = numpy.floor((nfft+1)*mel2hz(melpoints)/samplerate)

    fbank = numpy.zeros([nfilt,nfft//2+1])
    for j in range(0,nfilt):
        for i in range(int(bin[j]), int(bin[j+1])):
            fbank[j,i] = (i - bin[j]) / (bin[j+1]-bin[j])
        for i in range(int(bin[j+1]), int(bin[j+2])):
            fbank[j,i] = (bin[j+2]-i) / (bin[j+2]-bin[j+1])
    return fbank

def apply_full_mel(bins, fbank):
    filtered = numpy.matmul(fbank, bins)
    print(filtered.shape)
    return filtered

def generate_compact_mel(fbank):
    
    num_mels = fbank.shape[0]
    num_bins = fbank.shape[1]
    compact_mel = []

    #We use the following properties to generate the compact mels:
    #1) Only two mel filters overlap at any one time. Hence stride 2 through array
    #2) mel filter banks always add up to 1
    #So we only need to store the even MEL filter banks and no trailing zeros!

    first_bank_argmax = numpy.argmax(fbank[0])

    def get_shortended_line(line):
        first_non_zero_idx = next((i for i, x in enumerate(line) if x), None) 
        next_zero_idx = next((i for i, x in enumerate(line[first_non_zero_idx:]) if x == 0), None) + first_non_zero_idx
        # print(line)
        # print(first_non_zero_idx, next_zero_idx)
        shortended_line = line[first_non_zero_idx - 1:next_zero_idx]
        return shortended_line

    for idx in range(0, num_mels, 2):
        line = fbank[idx].tolist()
        print(idx)
        compact_mel.append(get_shortended_line(line))

    return compact_mel
    


def apply_compact_mel(bins, compact_fbank):
    max_mel = 1.0
    num_bins = bins.shape[0]
    filtered = numpy.zeros(num_bins)
    mel_even_idx = 0
    mel_odd_idx = 1
    mel_even_accum = 0
    mel_odd_accum = 0
    odd_mel_active_flag = False
    compact_fbank = [item for sublist in compact_fbank for item in sublist] #flatten
    compact_fbank.append(0.0)
    for idx in range(num_bins):
        even_mel = compact_fbank[idx]
        odd_mel = 0.0 if not odd_mel_active_flag else 1.0 - even_mel
        print(f"idx {idx}, mel_even_idx {mel_even_idx}, mel_odd_idx {mel_odd_idx}, even_mel {even_mel}, odd_mel {odd_mel}")
        mel_even_accum += bins[idx] * even_mel
        mel_odd_accum += bins[idx] * odd_mel

        if even_mel == 0.0 and idx > 0:
            filtered[mel_even_idx] = mel_even_accum
            mel_even_accum = 0
            mel_even_idx += 2

        if even_mel == max_mel:
            odd_mel_active_flag = True
            if mel_even_idx > 0:
                filtered[mel_odd_idx] = mel_odd_accum
                mel_odd_accum = 0
                mel_odd_idx += 2

    return numpy.array(filtered[0:49])


def main():
    fft_size = 512
    nbins = fft_size // 2 + 1
    fbank = get_filterbanks(49, fft_size, 16000)
    numpy.save("mel_filter", fbank)
    # grr = numpy.load("mel_filter.npy")
    # print(numpy.array_equal(fbank, grr))

    print(fbank.shape)
    old_nz = 0
    for bank in fbank.tolist():
        num_zeros = 0
        for value in bank:
            if value != 0.0:
                break;
            num_zeros += 1

        print(num_zeros, num_zeros - old_nz, bank[:40])
        old_nz = num_zeros

    test_bins = numpy.random.uniform(low=-1.0, high=1.0, size=(nbins))
    # test_bins = numpy.ones(nbins)

    print(test_bins.shape)
    full_filtered = apply_full_mel(test_bins, fbank)

    compact_mel = generate_compact_mel(fbank)
    compact_filtered = apply_compact_mel(test_bins, compact_mel)

    print(full_filtered[0:10], compact_filtered[0:10])

    print(numpy.isclose(full_filtered, compact_filtered))

if __name__ == "__main__":
    main()

