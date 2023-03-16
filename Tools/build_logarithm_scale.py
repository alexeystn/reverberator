import numpy as np

wave_height_px = 31
bit_depth = 16
zero_offset = 8

steps = np.exp(np.linspace(zero_offset,
                           bit_depth-1,
                           wave_height_px) * np.log(2))

steps_int = [int(i) for i in steps]

print('{', ', '.join([str(s) for s in steps_int]), '}')
