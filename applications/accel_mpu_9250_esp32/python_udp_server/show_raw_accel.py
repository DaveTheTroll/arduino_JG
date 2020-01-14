from accel_server import *

def onReading(data):
    print("A %s G %s T %d M %s t %d"%(
        str(data.accel),
        str(data.gyro),
        data.temperature,
        str(data.mag),
        data.timestamp))

s = accel_server(onReading)
s.run()
