import matplotlib.pyplot as plt
import numpy as np
import wave

filename = '20230108_122712.bin'

w = np.fromfile(filename, dtype='int16')

plt.plot(w[0::2])
plt.plot(w[1::2])
plt.show()

##filename = 'NewFile1'
##fragment_length_sec = 6
##osc_mem_size = 12e6
##sound_sample_rate = 48e3
##
##osc_sample_rate = osc_mem_size / 2 / fragment_length_sec
##filter_length = int(osc_sample_rate/sound_sample_rate)
##
##fir = np.ones(filter_length) / filter_length
##
##w = np.fromfile(filename + '.wfm', dtype='uint8')[-int(osc_mem_size)::2]
##w = w.astype('float')
##w -= np.mean(w)
##w = np.convolve(w, fir, mode='valid')
##w *= 256
##t = np.arange(len(w)) / osc_sample_rate
##ti = np.arange(int(sound_sample_rate * fragment_length_sec)) / sound_sample_rate
##wi = np.interp(ti, t, w).astype('int16')
##
##wav_wile = wave.open(filename + '.wav', 'w')
##wav_wile.setparams((1, 2, sound_sample_rate, 0, 'NONE', 'not compressed'))
##wav_wile.writeframes(wi.tobytes())
##wav_wile.close()
##
##plt.plot(wi)
##plt.show()
