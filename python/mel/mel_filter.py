import numpy
import sys
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt


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

    # add half of the last filter. This is needed because in the compact form, we
    # only store the even filters. While this is fine for an odd count, for even counts
    # we need to have the next half even filter from which we calculate the last odd filter
    idx_start = numpy.where(fbank[nfilt - 1] == numpy.max(fbank[nfilt - 1]))[0][0]
    new_mel_filter = numpy.zeros(nfft // 2 + 1)
    new_mel_filter[idx_start:] = 1.0 - fbank[nfilt - 1][idx_start:]
    fbank = numpy.vstack((fbank, new_mel_filter.T))


    if False:
        fig, axes = plt.subplots(1, ncols=1, figsize=[16, 6])
        fig.canvas.set_window_title("lowfreq = " + str(lowfreq))
      
        num_mels = fbank.shape[0]
        colours = ["r", "b", "g", "orange", "purple"]
        for idx in range(num_mels):
            axes.plot(fbank[idx], colours[idx%len(colours)], label=f"MEL {idx}")

        axes.set_xlim(0, nfft / 2 + 1)
        axes.legend(loc="upper right")

        # save to file
        plt.savefig(("MEL_" + str(lowfreq) + "Hz.png"), dpi=200)

        # close figure
        plt.close()

    return fbank

def apply_full_mel(bins, fbank):
    filtered = numpy.matmul(fbank, bins)
    # print(filtered.shape)
    return filtered

def get_shortended_line(line):
    first_non_zero_idx = next((i for i, x in enumerate(line) if x), None)
    # print(line)
    next_zero_idx = next((i for i, x in enumerate(line[first_non_zero_idx:]) if x == 0), None)
    next_zero_idx_or_end = next_zero_idx + first_non_zero_idx if next_zero_idx else len(line)
    # print(first_non_zero_idx, next_zero_idx_or_end)
    shortended_line = line[first_non_zero_idx - 1:next_zero_idx_or_end]
    # print(shortended_line)
    return shortended_line, first_non_zero_idx, next_zero_idx_or_end

def gen_c_line(int32_data):
    line = ""
    for data in int32_data:
        line += f"{data}, "
    return line

class compact_mel:
    def __init__(self, fbank):
        num_mels = fbank.shape[0]
        num_bins = fbank.shape[1]
        compact_mel = []

        #We use the following properties to generate the compact mels:
        #1) Only two mel filters overlap at any one time. Hence stride 2 through array
        #2) mel filter banks always add up to 1
        #So we only need to store the even MEL filter banks and no trailing zeros!
        #We create the pdd MEL filter from 1 - even

        for idx in range(0, num_mels, 2):
            line = fbank[idx].tolist()
            compact_mel.append(get_shortended_line(line)[0])
        if (num_mels % 2) == 0:
            line = fbank[num_mels-1].tolist()
            compact_mel.append(get_shortended_line(line)[0])

        # print(len([item for sublist in compact_mel for item in sublist]))
        self.compact_mel = compact_mel

    def filter(self, bins, filtered):
        compact_fbank = self.compact_mel
        max_mel = 1.0
        num_bins = bins.shape[0]
        mel_even_idx = 0
        mel_odd_idx = 1
        mel_even_accum = 0
        mel_odd_accum = 0
        odd_mel_active_flag = False
        compact_fbank = [item for sublist in compact_fbank for item in sublist] #flatten
        self.flattened_fbank = compact_fbank
        compact_fbank.append(0.0)
        for idx in range(num_bins):
            even_mel = compact_fbank[idx]
            odd_mel = 0.0 if not odd_mel_active_flag else 1.0 - even_mel

            # print(f"idx {idx}, mel_even_idx {mel_even_idx}, mel_odd_idx {mel_odd_idx}, even_mel {even_mel}, odd_mel {odd_mel}")
            mel_even_accum += bins[idx] * even_mel
            mel_odd_accum += bins[idx] * odd_mel

            if even_mel == 0.0 and idx > 0:
                filtered[mel_even_idx] = mel_even_accum
                # print("even", mel_even_accum)
                mel_even_accum = 0
                mel_even_idx += 2

            if even_mel == max_mel:
                odd_mel_active_flag = True
                if mel_even_idx > 0:
                    filtered[mel_odd_idx] = mel_odd_accum
                    # print("odd", mel_odd_accum)
                    mel_odd_accum = 0
                    mel_odd_idx += 2
        return filtered

    def count_mel_elements(self):
        count = 0
        # print(compact_mel)
        for filt in self.compact_mel:
            count += len(filt)
        return count

    def gen_c_src(self, var_name):
        c_text = f"int32_t {var_name}[{len(self.flattened_fbank)}] = {{"
        for line in self.compact_mel:
            c_text += gen_c_line(line)
        c_text = c_text[:-1] + "}};\n"
        
        return c_text
    
class compressed_mel:
    def __init__(self, fbank):

        num_mels = fbank.shape[0]
        num_bins = fbank.shape[1]
        compressed_mel = []

        #For compressed mels, we exploit the sparsity of the filters for each output mel
        #We find where the mel filter starts (skip zero elements) and ends (trailing zeros)
        #and just store the start/finish indicies and the non zero filter elements 

        for idx in range(0, num_mels):
            line = fbank[idx].tolist()
            shortended_line, first_non_zero_idx, next_zero_idx_or_end = get_shortended_line(line)
            compressed_mel.append((shortended_line[1:], first_non_zero_idx, next_zero_idx_or_end))

        self.compressed_mel = compressed_mel

    def filter(self, bins):
        compressed_fbank = self.compressed_mel
        num_mels = len(compressed_fbank)
        num_bins = bins.shape[0]

        filtered = numpy.zeros(num_mels)
        for idx in range(num_mels):
            shortended_line, first_non_zero_idx, next_zero_idx_or_end = compressed_fbank[idx]
            line_filt = numpy.array(shortended_line)
            filtered[idx] = numpy.dot(line_filt, bins[first_non_zero_idx:next_zero_idx_or_end])

        return filtered

    def count_mel_elements(self):
        count = 0
        for shortended_line, first_non_zero_idx, next_zero_idx_or_end in self.compressed_mel:
            count += len(shortended_line)
        return count




def test_equivalence(fft_size, nmels):
    print(f"testing fft_size: {fft_size}, n mels: {nmels}")
    nbins = fft_size // 2 + 1
    test_bins = numpy.random.uniform(low=-1.0, high=1.0, size=(nbins))
    # test_bins = numpy.ones(nbins)
    fbank = get_filterbanks(nmels, fft_size, 16000)
    fbank_standard = fbank[ :-1]
    full_filtered = apply_full_mel(test_bins, fbank_standard)
    
    my_compact_mel = compact_mel(fbank)
    compact_filtered = numpy.zeros(nmels)
    compact_filtered = my_compact_mel.filter(test_bins, compact_filtered)

    my_compressed_mel = compressed_mel(fbank_standard)
    compressed_filtered = my_compressed_mel.filter(test_bins)

    print(fbank_standard.size, my_compressed_mel.count_mel_elements(), my_compact_mel.count_mel_elements())

    # print(numpy.isclose(full_filtered, compact_filtered))
    # print(full_filtered, compact_filtered)
    result1 = numpy.allclose(full_filtered, compact_filtered)
    result2 = numpy.allclose(full_filtered, compressed_filtered)

    return result1 and result2

def main():
    for fft_size, test_mels in (
        (64, (5,6,7)),
        (128, range(5, 13)),
        (256, range(5, 29)),
        (512, range(5, 57)),
        (1024, range(10, 60))
        ):
        nbins = fft_size // 2 + 1
        for nmels in test_mels:
            assert test_equivalence(fft_size, nmels)
    print("PASS")

if __name__ == "__main__":
    main()

