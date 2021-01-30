import numpy as np
import subprocess
from mel import get_filterbanks, apply_full_mel

def single_test_equivalence(fft_size, nmels):
    print(f"testing fft_size: {fft_size}, n mels: {nmels}")
    nbins = fft_size // 2 + 1
    test_bins = np.random.uniform(low=-1.0, high=1.0, size=(nbins))
    # test_bins = np.ones(nbins)
    fbank = get_filterbanks(nmels, fft_size, 16000)
    fbank_standard = fbank[ :-1]
    full_filtered = apply_full_mel(test_bins, fbank_standard)
    
    my_compact_mel = compact_mel(fbank)
    compact_filtered = np.zeros(nmels)
    compact_filtered = my_compact_mel.filter(test_bins, compact_filtered)

    my_compressed_mel = compressed_mel(fbank_standard)
    compressed_filtered = my_compressed_mel.filter(test_bins)

    print(fbank_standard.size, my_compressed_mel.count_mel_elements(), my_compact_mel.count_mel_elements())

    # print(np.isclose(full_filtered, compact_filtered))
    # print(full_filtered, compact_filtered)
    result1 = np.allclose(full_filtered, compact_filtered)
    result2 = np.allclose(full_filtered, compressed_filtered)


def main():
    fft_size = 512
    nmels = 20
    nbins = fft_size // 2 + 1
    test_bins = np.random.uniform(low=-0x80000000, high=0x7ffffff, size=(nbins))
    int_test = test_bins.astype(np.int32)
    int_test.tofile("input.bin")
    cmd = "xsim --args app_equivalence 0 input.bin output.bin 1"
    result = subprocess.run(cmd.split(), stdout=subprocess.PIPE, text=True)
    print(result.stdout)
    fbank = get_filterbanks(nmels, fft_size, 16000)[ :-1] #note we lop of the last half bank used for compact only
    ref_result = apply_full_mel(test_bins, fbank)
    dut_result = np.fromfile("output.bin", dtype=np.int32, count=-1)

    print(ref_result.size, dut_result.size)
    print(ref_result, dut_result)


main()