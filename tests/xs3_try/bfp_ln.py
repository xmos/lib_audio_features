import math
import numpy as np 

def float_to_q28(fval):
    return int(fval * float(1<<24))

def q28_to_float(val):
    return float(float(val) / float(1<<24))

def q28_log(val):
    return float_to_q28(math.log(q28_to_float(val)))

def my_log(val):
    return q28_to_float(q28_log(float_to_q28(val)))
    

print(q28_log(float_to_q28(2)) * -25)
print(-(1<<31))

test_array = [float(i) for i in range(1, 10)]
print(test_array)
reference = [math.log(val) for val in test_array]
print(reference)
test_result = [my_log(val) for val in test_array]
print(test_result)

print(np.allclose(reference, test_result))



