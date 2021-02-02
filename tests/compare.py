import os
import numpy as np

num_features = 40
test_file_len_s = 10
test_file_freq = 200

os.system(f"sox -V -r 16000 -n -b 16 -c 1 input.wav synth {test_file_len_s} white {test_file_freq} vol -3dB")
os.system("sox input.wav input.raw")

ret = os.system('python3.7 bringup_py/features_test.py')
print(ret)
assert ret == 0, "py stuff failed"
ret = os.system('cd bringup_c && make && cd - && bringup_c/bin/app_features')
assert ret == 0, "c stuff failed"

#Note reshape to an array of n num_features
c_features = np.fromfile("features_c.bin", dtype=np.int8).reshape((-1, num_features))
py_features = np.fromfile("features_py.bin", dtype=np.int8).reshape((-1, num_features))

print("c: ", c_features.shape, " py: ", py_features.shape)

num_c = c_features.shape[1]
num_py = py_features.shape[1]
same = True
for idx in range(min(num_c, num_py)):
    match = np.allclose(c_features[idx,:], py_features[idx, :], atol=1, rtol=0.01)
    if not match:
        print(f"Failed at idx: {idx}")
        print(np.isclose(py_features[idx, :], c_features[idx, :]))
        print(c_features[idx, :])
        print(py_features[idx, :])
        same = False
if same:
    print("TEST PASSED")


