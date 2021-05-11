
#done with this fucking piece of SHIT !!!


# library & setup
## basic
import time
## serial
import serial
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev, 9600)
## mqtt
import paho.mqtt.client as paho


num = 0


# mqtt setup and some func.
## MQTT broker hosted on local machine
mqttc = paho.Client()
## Settings for connection
## TODO: revise host to your IP
host = "192.168.43.41"
topic = "Mbed"
## Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
    global num
    num = num + 1
    if (num%6)==1:
        s.write(bytes("/tilt/run\r", 'UTF-8'))
    elif (num%6)==0:
        s.write(bytes("/gesture/run\r", 'UTF-8'))
    else:
        print("haha")

'''
def on_message(mosq, obj, msg):
    if flag==1:
        print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
        flag = 2
    elif flag==3:
        print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
        flag = 0
    else:
        flag = flag
'''

'''
def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
'''        



## Set callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe
## Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)




#main part
#mqttc.publish(topic, "Message from Python!\n", qos=0)
time.sleep(1.5)
mqttc.loop_forever()

'''
flag=0
while 1:
    if flag==0:
        #print("flag 0\n")#.......test
        s.write(bytes("/gesture/run\r", 'UTF-8'))
        flag = 1 
    elif flag==1:
        #print("flag 1\n")#.......test
        flag = flag
    #mqttc.loop_forever()
    #time.sleep(10)
    #time.sleep(10)
    elif flag==2:
        #print("flag 2\n")#.......test
        s.write(bytes("/tilt/run\r", 'UTF-8'))
        flag = 3
    elif flag==3:
        #print("flag 3\n")#.......test
        flag = flag
    #time.sleep(10)
    #time.sleep(10)
    else:
        flag = flag
    time.sleep(0.1)
'''

'''
while 1:
    s.write(bytes("/gesture/run\r", 'UTF-8'))
    time.sleep(30)
    s.write(bytes("/tilt/run\r", 'UTF-8'))
    time.sleep(10)
'''


'''
s.write(bytes("\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 1 1\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 2 1\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 3 1\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 2 0\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 3 0\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/LEDControl/run 1 0\r", 'UTF-8'))
line=s.readline() # Read an echo string from mbed terminated with '\n' (putc())
print(line)
line=s.readline() # Read an echo string from mbed terminated with '\n' (RPC reply)
print(line)

s.close()
'''
