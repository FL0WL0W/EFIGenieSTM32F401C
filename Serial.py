import serial
import struct
ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE, 1)

readBytes = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])

def parseVoid():
    readBytes = ser.read(8)
    return "void"

def parseUINT8():
    readBytes = ser.read(8)
    return struct.unpack('B', readBytes[0])[0]
 
def parseUINT16():
    readBytes = ser.read(8)
    return struct.unpack('H', readBytes[0:1])[0]
 
def parseUINT32():
    readBytes = ser.read(8)
    return struct.unpack('I', readBytes[0:4])[0]
 
def parseUINT64():
    readBytes = ser.read(8)
    return struct.unpack('L', readBytes)[0]
 
def parseINT8():
    readBytes = ser.read(8)
    return struct.unpack('b', readBytes[0])[0]
 
def parseINT16():
    readBytes = ser.read(8)
    return struct.unpack('h', readBytes[0:1])[0]
 
def parseINT32():
    readBytes = ser.read(8)
    return struct.unpack('i', readBytes[0:4])[0]
 
def parseINT64():
    readBytes = ser.read(8)
    return struct.unpack('l', readBytes)[0]
 
def parseFloat():
    readBytes = ser.read(8)
    return struct.unpack('f', readBytes[0:4])[0]
 
def parseDouble():
    readBytes = ser.read(8)
    return struct.unpack('d', readBytes)[0]
 
def parseBool():
    readBytes = ser.read(8)
    if readBytes[0] != 0 :
        return "True"
    return "False"
  
def parseOther():
    readBytes = ser.read(8)
    return hex(readBytes[0]) + " " + hex(readBytes[1]) + " " + hex(readBytes[2]) + " " + hex(readBytes[3]) + " " + hex(readBytes[4]) + " " + hex(readBytes[5]) + " " + hex(readBytes[6]) + " " + hex(readBytes[7])
 
def parseBigOther():
    offset = int(input("Enter Offset: "))
    sendBytes = bytearray([offset, 0, 0, 0])
    ser.write(sendBytes)
    readBytes = ser.read(8)
    return hex(readBytes[0]) + " " + hex(readBytes[1]) + " " + hex(readBytes[2]) + " " + hex(readBytes[3]) + " " + hex(readBytes[4]) + " " + hex(readBytes[5]) + " " + hex(readBytes[6]) + " " + hex(readBytes[7])

while True:
    variableID = int(input("Enter id of variable to check: "))

    sendBytes = bytearray([variableID, 0, 0, 0])
    ser.write(sendBytes)
    readType = ser.read(1)
    if len(readType) > 0:
        switcher = {
            0: parseVoid,
            1: parseUINT8,
            2: parseUINT16,
            3: parseUINT32,
            4: parseUINT64,
            5: parseINT8,
            6: parseINT16,
            7: parseINT32,
            8: parseINT64,
            9: parseFloat,
            10: parseDouble,
            11: parseBool,
            12: parseBigOther,
            13: parseOther,
            14: parseBigOther
        }
        # Get the function from switcher dictionary
        func = switcher.get(readType[0], lambda: "Invalid Type")
        # Execute the function
        print(func())

ser.close()