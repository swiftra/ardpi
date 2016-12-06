import serial
import numpy
import time


ard = serial.Serial('/dev/ttyUSB0', 115200)


def getstatus():
    data = '01'
    ard.write(data)
    while ard.inWaiting() == 0:
        pass
    arduinoString = ard.readline()
    time.sleep(1)
    if arduinoString:
        data = arduinoString.split(',')
        print data
        dht1Temp = data[0]
        dht2Temp = data[1]
        dht1Hum = data[2]
        dht2Hum = data[3]
        ds1Temp = data[4]
        ds2Temp = data[5]
        tempSetting = data[6]
        humSetting = data[7]
        print dht1Temp
        print dht2Temp
        print dht1Hum
        print dht2Hum
        print ds1Temp
        print ds2Temp
        print tempSetting
        print humSetting
        time.sleep(5)


def sendcommand():
    cmd = '02'
    setting = ''
    setting = raw_input('Press 1 to change heater setting or press 2 to change humidity setting: ')
    if setting == "1":
        number = raw_input('Please enter a number between 1 and 99: ')
        if number >= '1' and number <= '99':
            ardString = cmd + setting + number
        else:
            print('Bad number')
    elif setting == "2":
        number = raw_input('Please enter a number between 1 and 99: ')
        if number >= '1' and number <= '99':
            ardString = cmd + setting + number
        else:
            print('Bad number')

    else:
        print('I made it to here and fucked up')

    ard.write(ardString)
    getstatus()
    print ardString

while True:
    com = raw_input('Press 1 to get status or press 2 to change settings: ')
    if com == '1' :
        getstatus()
    if com == '2':
        sendcommand()
    elif():
        print('invalid input DUMBASS')


