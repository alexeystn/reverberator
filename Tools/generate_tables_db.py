import numpy as np

n = 16

x_dB = (np.arange(n) - n + 3) * 3
# -inf ... -9, -6, -3, 0, +3, +6

y_dB = 10**(x_dB/20)
y_dB[0] = 0

text_dB = ['"{0:+4d}"'.format(i) for i in x_dB]
text_dB[0] = '"-inf"'

print('dB labels:')
print('{', ', '.join(text_dB), '}')

mult_dB = ['{0:.4f}'.format(i) for i in y_dB]

print('Log multipliers:')
print('{', ', '.join(mult_dB), '}')


ratio = 1 + np.arange(16) / 2.5
text_ratio = ['{0:.1f}'.format(i) for i in ratio]

print('Ratio:')
print('{', ', '.join(text_ratio), '}')
