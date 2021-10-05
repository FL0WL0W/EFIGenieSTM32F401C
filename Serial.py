import serial
import struct


def parse_readbytes(readBytes, readType):
    """Parse the bytes read off the serial console.

    Args:
        readBytes: The bytes data read off the console. Will always be 8 bytes long
            but sometimes only a portion of the bytes are valid depending on the readType.
        readType: Type of info the readBytes should be interpreted as. Mostly designates
            which struct the data should be unpacked into, but a couple of edge cases exist.

    Returns:
        readBytes parsed into a string or possibly a bool. 
    """
    # For the simple cases lets make a dictionary of the read type
    # key mapping to a tuple containing the struct format and the slice
    # of byte data that should be unpacked.
    fmtslc_switch = {
        1: ("B", slice(0, 1)),
        2: ("H", slice(0, 2)),
        3: ("I", slice(0, 4)),
        4: ("L", slice(0, 8)),
        5: ("b", slice(0, 1)),
        6: ("h", slice(0, 2)),
        7: ("i", slice(0, 4)),
        8: ("l", slice(0, 8)),
        9: ("f", slice(0, 4)),
        10: ("d", slice(0, 8)),
    }
    # For all cases we'll just use a simple if/else construct to parse
    if readType == 0:
        return "VOID"
    elif 1 <= readType <= 10:
        fmt, slc = switcher[readType][:]
        return struct.unpack(fmt, readBytes[slc])
    elif readType == 11:
        return bool(readBytes[0])
    elif 12 <= readType <= 14:
        return " ".join(readBytes[0:8])


def main():
    ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE, 1)
    readBytes = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])

    while True:
        variableID = input("Enter ID of variable to check (q to quit): ")
        if variableID.lower() == "q":
            break
        else:
            variableID = int(variableID)
        sendBytes = struct.pack("<I", variableID)
        ser.write(sendBytes)
        readType = ser.read(1)
        if readType == 12 or readType == 14:
            offset = int(input("Enter Offset: "))
            ser.write(struct.pack("<I", offset))
        readBytes = ser.read(8)
        print(parse_readbytes(readBytes, readType))

    ser.close()

if __name__ == "__main__":
    main()
