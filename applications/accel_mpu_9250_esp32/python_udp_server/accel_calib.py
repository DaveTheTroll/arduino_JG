from show_accel import show_accel
import pickle
import matplotlib.pyplot as plt
import matplotlib.widgets as mpl_widgets

def onReading(data, x, y, z):
    global ax, ay, az, collect
    if collect:
        ax = data.accel[0]
        ay = data.accel[1]
        az = data.accel[2]

        if ax > accel_calib[0] + accel_calib[1]:
            accel_calib[0] += (ax - (accel_calib[0] + accel_calib[1])) / 2
            accel_calib[1] = ax - accel_calib[0]
        elif ax < accel_calib[0] - accel_calib[1]:
            accel_calib[0] += (ax - (accel_calib[0] - accel_calib[1])) / 2
            accel_calib[1] = accel_calib[0] - ax

        if ay > accel_calib[2] + accel_calib[3]:
            accel_calib[2] += (ay - (accel_calib[2] + accel_calib[3])) / 2
            accel_calib[3] = ay - accel_calib[2]
        elif ay < accel_calib[2] - accel_calib[3]:
            accel_calib[2] += (ay - (accel_calib[2] - accel_calib[3])) / 2
            accel_calib[3] = accel_calib[2] - ay

        if az > accel_calib[4] + accel_calib[5]:
            accel_calib[4] += (az - (accel_calib[4] + accel_calib[5])) / 2
            accel_calib[5] = az - accel_calib[4]
        elif az < accel_calib[4] - accel_calib[5]:
            accel_calib[4] += (az - (accel_calib[4] - accel_calib[5])) / 2
            accel_calib[5] = accel_calib[4] - az

        collect = False

def accelCalib():
    return accel_calib

def onclick(event):
    global collect
    collect = True

def configFig(fig):
    global cid
    cid = fig.canvas.mpl_connect('button_press_event', onclick)

ax = []
ay = []
az = []
accel_calib = [0,0,0,0,0,0]
collect = False
show_accel(accelCalib, onReading, configFig)
pickle.dump(accel_calib, open('accel.calib','wb'))