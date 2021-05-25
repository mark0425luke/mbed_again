import serial
import time

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

s.write("+++".encode())
char = s.read(2)
print("Enter AT mode.")
print(char.decode())

s.write("ATMY 0x169\r\n".encode())
char = s.read(3)
print("Set MY 0X169.")
print(char.decode())

s.write("ATDL 0x269\r\n".encode())
char = s.read(3)
print("Set DL 0X269.")
print(char.decode())

s.write("ATID 0x1\r\n".encode())
char = s.read(3)
print("Set PAN ID 0X1.")
print(char.decode())

s.write("ATWR\r\n".encode())
char = s.read(3)
print("Write config.")
print(char.decode())

s.write("ATMY\r\n".encode())
char = s.read(4)
print("MY :")
print(char.decode())

s.write("ATDL\r\n".encode())
char = s.read(4)
print("DL : ")
print(char.decode())

s.write("ATCN\r\n".encode())
char = s.read(3)
print("Exit AT mode.")
print(char.decode())


print("start sending RPC")


#s.write(bytes("/accelero/run 0\r", 'UTF-8'))
s.write("/getAcc/run 0\r".encode())

#haha = s.read(22)
#print(haha.decode())


while True:
    haha = s.read(26)
    print(haha.decode('unicode_escape'))
    time.sleep(1)
    time.sleep(1)


'''
while True:
    s.write("/myled1/write 1\r".encode())
    time.sleep(1)
    s.write("/myled1/write 0\r".encode())
    time.sleep(1)
'''

s.close()
    

    