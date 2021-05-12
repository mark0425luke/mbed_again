
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
## plot
import matplotlib.pyplot as plt
import numpy as np


#mine: variable
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
    if (num%11)==1:
        #send command
        s.write(bytes("/retrieve/run\r", 'UTF-8'))

        #plot, not yet
        np.linspace(0, 12, 256, endpoint=True)
        x,y = np.cos(X), np.sin(X)
        plt.plot(X,x)
        plt.plot(X,y)
        plt.show()  

    else:
        num = num



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
