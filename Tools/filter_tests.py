import ctypes
from enum import Enum
import numpy as np
from matplotlib import pyplot as plt

# build .so from .c source:
# gcc -shared -o filter.so ../Firmware/Sound/biquad.c

lib = ctypes.CDLL('filter.so')


class BiquadFilter(ctypes.Structure):
    _fields_ = [
        ('ab', ctypes.c_float * 5),
        ('xy', ctypes.c_float * 4),
        ]


class FilterType(Enum):
    LPF = 0
    HPF = 1
    NOTCH = 2
    BPF = 3


SAMPLE_RATE = 48000
TIME_LENGTH = 3

# Adjustable parameters:
filter_type = FilterType.HPF
quality_factor = 1 / np.sqrt(2)
cutoff_frequency = 260
min_frequency = 10
max_frequency = 10e3
points_per_decade = 10


frequencies = 10**(np.arange(np.log10(min_frequency) * points_per_decade,
                             np.log10(max_frequency) * points_per_decade) /
                   points_per_decade)
amplitude_response = np.zeros(frequencies.shape)

t = np.arange(SAMPLE_RATE * TIME_LENGTH) / SAMPLE_RATE
output = np.zeros(t.shape, dtype='float32')

lib.biquadFilterApply.restype = ctypes.c_float

biquadFilter = BiquadFilter()

for i_freq, freq in enumerate(frequencies):

    lib.biquadFilterInit(ctypes.byref(biquadFilter),
                         ctypes.c_float(cutoff_frequency),
                         ctypes.c_uint32(int(1e6 / SAMPLE_RATE)),
                         ctypes.c_float(quality_factor),
                         ctypes.c_uint32(filter_type.value)
                         )

    input_signal = np.sin(2 * np.pi * freq * t)

    for i_smp, sample in enumerate(input_signal):
        output[i_smp] = lib.biquadFilterApply(ctypes.byref(biquadFilter),
                                              ctypes.c_float(sample))

    amplitude_response[i_freq] = np.mean(output**2) * 2
    print(i_freq)

plt.plot(frequencies, amplitude_response)

title = '{0}, F={1:.0f}, Q={2:.1f}'.format(filter_type.name,
                                           cutoff_frequency,
                                           quality_factor)
plt.xscale('log')
plt.yscale('log')
plt.ylim((1e-3, 5))
plt.xlabel('Frequency, Hz')
plt.ylabel('Amplitude')
plt.grid(True, which='both')
plt.title(title)
plt.savefig(title.lower() + '.png')
plt.show()
