
import smbus
import time

bus = smbus.SMBus(1)

address = 0x29
x = list()
a = 0
b = 0
c = 0
d = 0


def writeNumber(a,b,c,d):
    bus.write_i2c_block_data(address, a, [b,c,d])
    return -1

def readNumber():


    x.append(bus.read_i2c_block_data(address, 4))
    return x



while True:
    print ('Press 1 to adjust temp:')
    print ('Press 2 to adjust humidity:')
    print ('Press 3 to recieve temp and humidity')
    print ('Press 4 to turn on led 1')
    print ('Press 5 to turn on led 2')
    print ('Press 6 to turn on led 3')
    print ('Press 7 to turn off led 1')
    print ('Press 8 to turn off led 2')
    print ('Press 9 to turn off led 3')


    a = input('What would you like to do:')
    if a == 1:
        b = input('What would you like to adjust the temp to?')
    if a == 2:
        b = input('What would you like to adjust humidity to?')
    if a == 3:
        print ('Okay')
    if a == 4:
        b = 1
    if a == 5:
        b = 1
    if a == 6:
        b = 1


    writeNumber(a, b, c, d)
    time.sleep(2)
    readNumber()
    for index, item in enumerate(x):
        print (index, item)

    print x







