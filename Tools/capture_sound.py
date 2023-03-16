import time
import serial
import serial.tools.list_ports
from datetime import datetime
import numpy as np
from matplotlib import pyplot as plt
import wave

available_ports = [c.device for c in serial.tools.list_ports.comports()]
for port in available_ports:
    print(port)

port_name = '/dev/cu.usbserial-A50285BI' # FT232
port_name = '/dev/cu.usbserial-0001' # SP2102
port_name = '/dev/cu.usbserial-FT2TLUOA0' # FTDI

capture_time = 15  # seconds

filename = datetime.now().strftime('%Y%m%d_%H%M%S.bin')

with open(filename, 'wb') as f:
    with serial.Serial(port_name, baudrate=3000000, timeout=1) as ser:
        total_bytes = 0
        t_stop = time.time() + capture_time
        
        ser.flush()
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        
        while (time.time() < t_stop):
            d = ser.read(1000)
            f.write(d)
            total_bytes += len(d)

print(total_bytes, 'bytes captured')
    
d = np.fromfile(filename, dtype='int16')

d = d.reshape((-1,2)) * 32

for i in range(2):
    
    wav_wile = wave.open(filename.split('.')[0] + '_' + str(i) + '.wav', 'w')
    wav_wile.setparams((1, 2, 48000, 0, 'NONE', 'not compressed'))
    wav_wile.writeframes(d[:,i].tobytes())  # w[i::2]
    wav_wile.close()    

plt.plot(d)
plt.show()
