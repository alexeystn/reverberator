import time
import serial
import serial.tools.list_ports
from datetime import datetime

available_ports = [c.device for c in serial.tools.list_ports.comports()]
for port in available_ports:
    print(port)

port_name = '/dev/cu.usbserial-A50285BI'

filename = datetime.now().strftime('%Y%m%d_%H%M%S.bin')


with serial.Serial(port_name, baudrate=3000000, timeout=1) as ser:
    with open(filename, 'wb') as f:
        total_bytes = 0
        t_stop = time.time() + 4
        
        while (time.time() < t_stop):
            d = ser.read(1024*16)
            f.write(d)
            total_bytes += len(d)

print(total_bytes, 'bytes captured')
    
