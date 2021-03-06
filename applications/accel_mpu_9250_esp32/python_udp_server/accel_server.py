import socket
import sys
import struct
from math import sqrt

def hypotn(x):
    return sqrt(sum([a*a for a in x]))
class accel_data:
    def __init__(self, buf):
        self.buf = buf

    @property
    def accel(self): return struct.unpack('h'*3, self.buf[0:6])         # 8192 -> 4g
    @property
    def gyro(self): return struct.unpack('h'*3, self.buf[6:12])         # 8192 -> 1000deg/s
    @property
    def temperature(self): return struct.unpack('h', self.buf[12:14])[0]
    @property
    def mag(self):
        m = struct.unpack('h'*3, self.buf[14:20])         # 8192-> 4.8mT
        return (m[1],m[0],m[2])    # X&Y switched, Z inverted
    @property
    def timestamp(self): return struct.unpack('Q', self.buf[20:28])[0]  # us

class accel_server:
    def __init__(self, reading_callback):
        self.reading_callback = reading_callback

        try :
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.settimeout(2)
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except socket.error as msg:
            print ('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
            sys.exit()

        try:
            self.sock.bind(('', 49198))
        except socket.error as msg:
            print ('Bind failed. Error: ' + str(msg[0]) + ': ' + msg[1])
            sys.exit()

        self.exit = False

    def run(self):
        while not self.exit:
            try:
                buf = self.sock.recvfrom(1024)

                self.reading_callback(accel_data(buf[0]))

            except socket.timeout:
                pass

        self.sock.close()
