from accel_server import *
import threading

axes = ("X","Y","Z")
ud_s = ("up", "down")
lr_s = ("right", "left")

def onReading(data):
    global collect, i, vert, horz, lr, ud
    if collect > 0:
        print("%3d A [%6.0f %6.0f %6.0f] G [%6.0f %6.0f %6.0f] T %d M [%6.0f %6.0f %6.0f] t %d |A| %6.0f |M| %6.0f"%(
            collect,
            data.accel[0], data.accel[1], data.accel[2],
            data.gyro[0], data.gyro[1], data.gyro[2],
            data.temperature,
            data.mag[0], data.mag[1], data.mag[2],
            data.timestamp,
            hypotn(data.accel),
            hypotn(data.mag)
            ))
        line = "%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n"%(
                    axes[vert], ud_s[ud], axes[horz], lr_s[lr],
                    data.accel[0], data.accel[1], data.accel[2],
                    data.gyro[0], data.gyro[1], data.gyro[2],
                    data.temperature,
                    data.mag[0], data.mag[1], data.mag[2],
                    data.timestamp)
        line = line.replace("(","")
        line = line.replace(")","")
        csv.write(line)
        collect -= 1
        if collect == 0:
            nextMessage()

def runAccel():
    s.run()

def nextMessage():
    global i, vert, horz, lr, ud
    
    vert = -1
    horz = -1
    while vert == horz:
        i += 1
        horz = int(i) % 3
        lr = int(i/3) % 2
        ud = int(i/6) % 2
        vert = int(i/12) % 3

    if i < 36:
        print("%s %s.  %s %s"%(axes[vert], ud_s[ud], axes[horz], lr_s[lr]))
    else:
        s.exit = True

try:
    s = accel_server(onReading)

    i = -1
    collect = 0
    nextMessage()

    csv = open("calib.csv", "w")

    thread = threading.Thread(target=runAccel)
    thread.start()

    x = ""
    while not s.exit and x != 'x':
        x = input()
        if collect == 0:
            collect = 200
finally:
    s.exit = True