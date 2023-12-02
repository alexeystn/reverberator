import numpy as np
from matplotlib import pyplot as plt


def generate_curve(num, min_val, max_val, exp):
    y =  2 ** (np.arange(num)/exp) - 1
    y /= y[-1]
    y *= max_val - min_val
    y += min_val
    return y

    
y = generate_curve(16, 0, 1, 6)
print('tableLevels')
print(', '.join(['{0:.3f}'.format(i) for i in y]))

y = generate_curve(16, 1, 5, 6)
print('tableRatios')
print(', '.join(['{0:.3f}'.format(1/i) for i in y]))
print(', '.join(['{0:.3f}'.format(i) for i in y]))

y = generate_curve(16, 500, 20000, 6)[::-1]
print('tableThresholds')
print(', '.join(['{0:5.0f}'.format(iy) for iy in y]))


y = generate_curve(16, 200, 20000, 3)
print('tableCutoffFreq')
print(', '.join(['{0:5.0f}'.format(iy) for iy in y]))

plot_y = y

plt.plot(plot_y, '.-');
plt.show()
    






        







        
