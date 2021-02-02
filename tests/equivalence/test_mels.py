import numpy as np
import subprocess
from mel import get_filterbanks, apply_full_mel, compact_mel, compressed_mel
from pathlib import Path
import os

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
    assert np.allclose(full_filtered, compact_filtered)
    assert np.allclose(full_filtered, compressed_filtered)


def do_test_run(fft_size, nmels, type):
    nbins = fft_size // 2 + 1
    # test_bins = np.random.uniform(low=-1.0, high=1.0, size=(nbins))
    test_bins = np.ones(nbins) #Use this for debugging

    fbank = get_filterbanks(nmels, fft_size, 16000) 
    ref_result = apply_full_mel(test_bins, fbank[ :-1])#note we lop of the last half bank used for compact only

    if type == "compact":    
        my_compact_mel = compact_mel(fbank)
        name = f"mel_filter_{fft_size}_{nmels}_compact"
        c_text = my_compact_mel.gen_c_src(name)

    elif type == "compressed":
        my_compressed_mel = compressed_mel(fbank)
        name = f"mel_filter_{fft_size}_{nmels}_compressed"
        c_text = my_compressed_mel.gen_c_src(name)

    else:
        assert 0 & f"unknown embedded mel type: {type}"

    with open(Path("src") / (name + ".h"), "wt") as hfile:
        hfile.write(c_text)
    os.environ['MEL_FILTER_H_FILE'] = name + ".h"

    result = subprocess.run("cmake .".split(), stdout=subprocess.PIPE, text=True)
    result = subprocess.run("make -j".split(), stdout=subprocess.PIPE, text=True)

    int_test = (test_bins * np.iinfo(np.int32).max).astype(np.int32)
    int_test.tofile("input.bin")
    cmd = "xsim --args app_equivalence 0 input.bin output.bin 1"
    result = subprocess.run(cmd.split(), stdout=subprocess.PIPE, text=True)
    print(result.stdout)
    dut_result = np.fromfile("output.bin", dtype=np.int32, count=-1)

    if type == "compact":    
        dut_result = dut_result.astype(np.double) /  np.iinfo(np.int32).max * (1 << my_compact_mel.get_headroom_bits())
    else:
        dut_result = dut_result.astype(np.double) /  np.iinfo(np.int32).max * (1 << my_compressed_mel.get_headroom_bits())
    
    # print(ref_result.size, dut_result.size)
    # print(ref_result, "\n", dut_result)

    rtol=0.0000001 
    # print(np.isclose(ref_result, dut_result, rtol=rtol))
    assert(np.allclose(ref_result, dut_result, rtol=rtol))
    print("TEST PASS")

def test_mels():
    fft_size = 512
    nmels = 49

    do_test_run(fft_size, nmels, "compact")
    do_test_run(fft_size, nmels, "compressed")
 
    nmels = 20
    do_test_run(fft_size, nmels, "compact")
    do_test_run(fft_size, nmels, "compressed")

if __name__ == "__main__":
    test_mels()