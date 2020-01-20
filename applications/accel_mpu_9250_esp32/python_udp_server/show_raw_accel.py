from accel_server import *
from math import sqrt

def hypotn(x):
    return sqrt(sum([a*a for a in x]))

def onReading(data):
    print("A [%6.0f %6.0f %6.0f] G [%6.0f %6.0f %6.0f] T %d M [%6.0f %6.0f %6.0f] t %d |A| %6.0f |M| %6.0f"%(
        data.accel[0], data.accel[1], data.accel[2],
        data.gyro[0], data.gyro[1], data.gyro[2],
        data.temperature,
        data.mag[0], data.mag[1], data.mag[2],
        data.timestamp,
        hypotn(data.accel),
        hypotn(data.mag)
        ))

s = accel_server(onReading)
s.run()
