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
port_name = '/dev/cu.usbserial-FTYZOWYP0'

capture_time = 5  # seconds

filename = 'Capture/' + datetime.now().strftime('%Y%m%d_%H%M%S.bin')

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
    
d = np.fromfile(filename, dtype='uint8')


for i in range(4):
    m = d[i::4].min()
    print(m)
    if m == 1:
        start = i
    
n = (len(d) - 16) // 4

d = d[start:start+n*4]

signal_in = (d[0::4] + d[1::4]*256).astype('int16')
signal_out = (d[2::4] + d[3::4]*256).astype('int16')

for i, sig in enumerate([signal_in, signal_out]):
    
    wav_wile = wave.open(filename.split('.')[0] + '_' + str(i) + '.wav', 'w')
    wav_wile.setparams((1, 2, 48000, 0, 'NONE', 'not compressed'))
    wav_wile.writeframes(sig.tobytes())  # w[i::2]
    wav_wile.close()    

plt.plot(signal_in)
plt.plot(signal_out)
plt.grid(True)
plt.show()
