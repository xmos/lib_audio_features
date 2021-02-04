import numpy as np
import subprocess
from mel import get_filterbanks, apply_full_mel, compact_mel, compressed_mel
from pathlib import Path
import os

def float_to_s32bfp(float_array):
    max_mag = np.amax(np.absolute(float_array))
    exp = np.ceil(np.log2(max_mag))
    exp -= 31
    mant_vect = (float_array / 2**exp).astype(np.int32)
    bfp_vect = np.concatenate((np.atleast_1d(exp), mant_vect))
    return bfp_vect.astype(np.int32)

def s32bfp_to_float(bfp):
    float_array = bfp.astype(np.float64)[1:]
    exp = bfp[0]
    float_array *= 2.0**exp
    return float_array


def do_test_run(ln_vect_len, max_in=np.finfo(np.float32).max, rtol=0.000001):
  
    os.environ['LN_TEST_VECTOR_LENGTH'] = str(ln_vect_len)

    result = subprocess.run("cmake .".split(), stdout=subprocess.PIPE, text=True)
    result = subprocess.run("make -j".split(), stdout=subprocess.PIPE, text=True)

    test_vect = np.random.uniform(low=0, high=max_in, size=(ln_vect_len)).astype(np.float32)
    
    ref_results = np.log(test_vect)

    bfp_array = float_to_s32bfp(test_vect)
    assert np.allclose(test_vect, s32bfp_to_float(bfp_array))
    bfp_array.tofile("input.bin")
    cmd = "xsim --args app_equivalence input.bin output.bin 1"
    result = subprocess.run(cmd.split(), stdout=subprocess.PIPE, text=True)
    print(result.stdout)

    dut_result = np.fromfile("output.bin", dtype=np.int32, count=-1)
    dut_result = s32bfp_to_float(dut_result)
    
    # print(ref_results.size, dut_result.size)
    # print(ref_results, "\n", dut_result)

    atol=1e-6
    allclose = np.allclose(ref_results, dut_result, rtol=rtol, atol=atol)
    if not allclose:
        for ref, dut in np.c_[ref_results, dut_result]:
            print(ref, dut, np.isclose(ref, dut, rtol=rtol, atol=atol), "{:.20f}".format(abs(1-ref/dut)), ref-dut)
        assert()
    print("TEST PASS")

def test_ln():
    ln_vect_len = 512

    do_test_run(ln_vect_len, max_in=np.finfo(np.float32).max)
    do_test_run(ln_vect_len, max_in=2.0**63)
    do_test_run(ln_vect_len, max_in=2.0**31)
    do_test_run(ln_vect_len, max_in=2.0**15)
    do_test_run(ln_vect_len, max_in=2.0**7)
    do_test_run(ln_vect_len, max_in=1.0, rtol=0.00001)
    do_test_run(ln_vect_len, max_in=2.0**-7, rtol=0.00001)
    do_test_run(ln_vect_len, max_in=2.0**-15, rtol=0.000001)
    do_test_run(ln_vect_len, max_in=2.0**-31, rtol=0.000001)
    do_test_run(ln_vect_len, max_in=np.finfo(np.float32).eps, rtol=0.000001)


if __name__ == "__main__":
    test_ln()
