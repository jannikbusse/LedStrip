import serial
import struct
import time
import socket
import re
import datetime
from datetime import datetime
from datetime import timedelta
from datetime import time
arduino = serial.Serial(port='/dev/ttyUSB0',   baudrate=9600, timeout=.1)
#arduino.close()



#arduino.write(struct.pack('b', ord('S')))
#arduino.write(struct.pack('=bf', ord('M'), float(10)))


def is_winter_time():
    return True

UDP_IP = "192.168.178.114"
UDP_PORT = 1234

def parse_msg(msg, raw):
    if msg == "F":
        arduino.open()
        arduino.write(struct.pack('=b', ord('F')))
        arduino.close()
    if msg[0:1] == "O":
        arduino.open()
        arduino.write(struct.pack('=bf', ord('O'), float(msg[1:])/1000.0))
        arduino.close()
    if msg[0:1] == "V":
        #arduino.open()
        arduino.write(struct.pack('=bf', ord('V'), float(msg[1:])))
        #arduino.close()
    if msg[0:1] == "T":
        arduino.open()
        #arduino.write(struct.pack('=bf', ord('T'), float(msg[1:]))
        print(msg[1:])
        hrs = msg[1:3]
        min = msg[4:6]
        t1 = datetime.utcnow()
        if is_winter_time():
            dt = timedelta(hours=1)
            t1 = t1 + dt
        print(t1)
        t2 = datetime.utcnow()
        t2 = t2.replace(hour=int(hrs), minute=int(min), second=0)
        delta = t2 -t1
        dy = timedelta(days=1)
        if(delta.total_seconds() < 0):
            print("lala")
            delta = delta +dy 
        print(int(delta.total_seconds()))
        arduino.write(struct.pack('=bL', ord('T'), int(delta.total_seconds())))
        arduino.close()
    if msg[0:1] == "A":
        arduino.open()
        arduino.write(b'A' + bytes.fromhex(msg[1:]))
        arduino.close()
        print(bytes.fromhex(msg[1:]))
    if msg == "S":
        arduino.open()
        arduino.write(struct.pack('=b', ord('S')))
        arduino.close()
    if msg == "C":
        arduino.open()
        arduino.write(struct.pack('=b', ord('C')))
        arduino.close()
    if msg[0:len("Brightness")] == "Brightness":
        print( int(msg[len("Brightness"):]))
        arduino.open()
        arduino.write(struct.pack('=bf', ord('B'), float(msg[len("Brightness"):])))
        arduino.close()



with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as sock:
    sock.bind((UDP_IP, UDP_PORT))
    while True:
        data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
        parse_msg(str(data, "utf-8"), data)
sock.close()
