from show_mag import show_mag
import pickle

mag_calib = [0,0,0,0,0,0]

def onReading(data,x,y,z):
    global mag_calib
    mx = data.mag[0]
    my = data.mag[1]
    mz = -data.mag[2]

    #print(("%1.3f %1.3f %1.3f"%(mx, my, mz)))

    prev = mag_calib.copy()

    if mx > mag_calib[0] + mag_calib[1]:
        mag_calib[0] += (mx - (mag_calib[0] + mag_calib[1])) / 2
        mag_calib[1] = mx - mag_calib[0]
    elif mx < mag_calib[0] - mag_calib[1]:
        mag_calib[0] += (mx - (mag_calib[0] - mag_calib[1])) / 2
        mag_calib[1] = mag_calib[0] - mx

    if my > mag_calib[2] + mag_calib[3]:
        mag_calib[2] += (my - (mag_calib[2] + mag_calib[3])) / 2
        mag_calib[3] = my - mag_calib[2]
    elif my < mag_calib[2] - mag_calib[3]:
        mag_calib[2] += (my - (mag_calib[2] - mag_calib[3])) / 2
        mag_calib[3] = mag_calib[2] - my

    if mz > mag_calib[4] + mag_calib[5]:
        mag_calib[4] += (mz - (mag_calib[4] + mag_calib[5])) / 2
        mag_calib[5] = mz - mag_calib[4]
    elif mz < mag_calib[4] - mag_calib[5]:
        mag_calib[4] += (mz - (mag_calib[4] - mag_calib[5])) / 2
        mag_calib[5] = mag_calib[4] - mz

def magCalib():
    global mag_calib
    return mag_calib

show_mag(magCalib, onReading)
pickle.dump(mag_calib, open('mag.calib','wb'))
