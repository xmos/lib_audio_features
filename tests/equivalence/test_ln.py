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


def do_test_run(ln_vect_len, exp):
  
    os.environ['LN_TEST_VECTOR_LENGTH'] = str(ln_vect_len)

    result = subprocess.run("cmake .".split(), stdout=subprocess.PIPE, text=True)
    result = subprocess.run("make -j".split(), stdout=subprocess.PIPE, text=True)

    test_vect = np.random.uniform(low=np.finfo(np.float32).min, high=np.finfo(np.float32).max, size=(ln_vect_len)).astype(np.float32)
    bfp_array = float_to_s32bfp(test_vect)
    assert np.allclose(test_vect, s32bfp_to_float(bfp_array))

    bfp_array.tofile("input.bin")
    cmd = "xsim --args app_equivalence input.bin output.bin 1"
    result = subprocess.run(cmd.split(), stdout=subprocess.PIPE, text=True)
    print(result.stdout)
    dut_result = np.fromfile("output.bin", dtype=np.int32, count=-1)
    print(dut_result)
    dut_result = s32bfp_to_float(dut_result)
    
    print(test_vect.size, dut_result.size)
    print(test_vect, "\n", dut_result)

    rtol=0.0000001 
    assert(np.allclose(test_vect, dut_result, rtol=rtol))
    print("TEST PASS")

def test_ln():
    ln_vect_len = 27

    do_test_run(ln_vect_len, 0)

if __name__ == "__main__":
    test_ln()
