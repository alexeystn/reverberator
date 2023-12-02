import numpy as np
from matplotlib import pyplot as plt

F_COMB = 0
F_ALLPASS = 1

class Filter:

    def __init__(self, ftype, gain, delay_ms):

        length = int(48e3 * delay_ms / 1e3)
        self.length = length
        self.gain = gain
        self.ftype = ftype
        self.pointer = 0
        self.buffer = np.zeros((length,))

    def apply(self, sample):

        readback = self.buffer[self.pointer]
        if self.ftype == F_COMB:
            new = readback * self.gain + sample
        else:
            readback += -self.gain * sample
            new = readback * self.gain + sample
        self.buffer[self.pointer] = new
        self.pointer += 1
        if self.pointer == self.length:
            self.pointer = 0
        return readback

class Reverb:

    def __init__(self, combK, allK):
        self.filtersComb = []
        self.filtersAllPass = []

        for k in combK:
            self.filtersComb.append(Filter(F_COMB, k[0], k[1]))
        for k in allK:
            self.filtersAllPass.append(Filter(F_ALLPASS, k[0], k[1]))

    def apply(self, sample):
        newSample = 0
        for filt in self.filtersComb:
            newSample += filt.apply(sample)
        newSample /= len(self.filtersComb)
        for filt in self.filtersAllPass:
            newSample = filt.apply(newSample)
        return newSample

    def response(self):
        x = np.zeros((100000,))
        x[0] = 1
        for i in range(len(x)):
            x[i] = self.apply(x[i])
        return x

allPass_params = [(0.7, 5.00), (0.7, 1.68), (0.7, 0.48)]
default_coefs = [0.805, 0.827, 0.783, 0.764]
default_delays = [36.04, 31.12, 40.44, 44.92]

exp = np.exp(np.linspace(1.3, -1.3, 16))

print('tableReverbGains')
for i in range(len(exp)):
    print('{', ', '.join(['{0:.3f}'.format(default_coefs[j]**exp[i]) for j in range(4)]), '},')
    comb_params = [(default_coefs[j]**exp[i], default_delays[j]) for j in range(4)]
    r = Reverb(comb_params, allPass_params)
    plt.plot(r.response() + i *0.3)

plt.grid(True)
plt.show()



    




        







        
