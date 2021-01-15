import os
import numpy as np

num_features = 40
test_file_len_s = 10
test_file_freq = 10

os.system(f"sox -V -r 16000 -n -b 16 -c 1 sin.wav synth {test_file_len_s} sin {test_file_freq} vol -3dB")
os.system("sox sin.wav sin.raw")

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
for idx in range(min(num_c, num_py)):
    # print(np.isclose(c_features[:,idx], py_features[:,idx]))
    print(c_features[idx, 0:12], py_features[idx, 0:12])
