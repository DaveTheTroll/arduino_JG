from accel_server import *
import threading

axes = ("X","Y","Z")
ud_s = ("up", "down")
lr_s = ("right", "left")

def onReading(data):
    global collect, i, vert, horz, lr, ud
    if collect:
        print("A %s G %s T %d M %s t %d"%(
            str(data.accel),
            str(data.gyro),
            data.temperature,
            str(data.mag),
            data.timestamp))
        line = "%s,%s,%s,%s,%s,%s,%d,%s,%d\n"%(
                    axes[vert], ud_s[ud], axes[horz], lr_s[lr],
                    str(data.accel),
                    str(data.gyro),
                    data.temperature,
                    str(data.mag),
                    data.timestamp)
        line = line.replace("(","")
        line = line.replace(")","")
        csv.write(line)
        collect = False
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
    collect = False
    nextMessage()

    csv = open("calib.csv", "w")

    thread = threading.Thread(target=runAccel)
    thread.start()

    x = ""
    while not s.exit and x != 'x':
        x = input()
        collect = True
finally:
    s.exit = True