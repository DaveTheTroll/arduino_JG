import socket
import sys
import struct

print("Hello")

try :
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(2)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
except socket.error as msg:
    print ('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
    sys.exit()

try:
    s.bind(('', 49198))
except socket.error as msg:
    print ('Bind failed. Error: ' + str(msg[0]) + ': ' + msg[1])
    sys.exit()
     
print ('Server listening')

while 1:
    try:
        buf = s.recvfrom(1024)
        count = int(len(buf[0])/2)
        data = struct.unpack('h'*count, buf[0])

        print(data)

    except socket.timeout:
        print('.')
    
s.close()