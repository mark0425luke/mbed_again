import serial
import time

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)


while True:
    haha = s.read(6)
    print(haha.decode('unicode_escape'))
    #print(haha.decode())
    time.sleep(1)


s.close()
    

    