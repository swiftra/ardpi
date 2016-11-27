import serial
import time

arduino = serial.Serial('/dev/ttyUSB0', 9600)
time.sleep(2)
i = 0

while i < 4:
    setCommand = 1
    setNextCommand = 1
    setAction = 0
    setData = 0
    arduino.flush()
    command = str(setCommand)
    nextCommand = str(setNextCommand)
    action = str(setAction)
    data = str(setData)
    print ('Python values sent: ')
    print (command)
    print (nextCommand)
    print (action)
    print (data)

    arduino.write(command)
    arduino.write(nextCommand)
    arduino.write(action)
    arduino.write(data)
    time.sleep(1)
    msg = arduino.read(arduino.inWaiting())
    print (msg)

    time.sleep(2.5)
    i = i+1

else:
    print ('this is bullshit #2')
exit()
