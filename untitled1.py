import serial
import time

mydata = list()
arduino = serial.Serial('/dev/ttyUSB0',9600)

while (1==1):
    if(arduino.inWaiting()>0):
        myData = arduino.readline()
        print myData




    time.sleep(5)
