import socket
import sys
import struct

class accel_server:
    def __init__(self):
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
                count = int(len(buf[0])/2)
                data = struct.unpack('h'*count, buf[0])

                print(data)

            except socket.timeout:
                pass

        self.sock.close()
