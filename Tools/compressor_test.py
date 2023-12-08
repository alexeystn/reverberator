import numpy as np
from matplotlib import pyplot as plt
import os

# gcc main.c compressor.c -o comp.exe
# cd C_test
# gcc main.c ../../Firmware/Sound/compressor.c -o cmp

fs = 48800
f = 10000
threshold = 3000
ratio = 3
time = 1

t = np.arange(0, time*fs) / fs
x = np.sin(2 * np.pi * f * t) * 2500
x += (np.random.random(time*fs) - 0.5)*50

x[(t > 0.2) * (t < 0.3)] *= 2
x[(t > 0.5) * (t < 0.7)] *= 7
x[(t > 0.55) * (t < 0.65)] *= 1.3

x = x.astype('int16')

np.savetxt('input.txt', x, '%d', '\n')

cmd = './C_test/cmp {0} {1} < input.txt > output.txt'.format(threshold, ratio)
print(cmd)
os.system(cmd)

xx = np.genfromtxt('output.txt')

plt.plot(t, x)
plt.plot(t, xx)
plt.plot([0, time], [0, 0], ':k')
plt.plot([0, time], [threshold, threshold], ':k')
plt.show()

