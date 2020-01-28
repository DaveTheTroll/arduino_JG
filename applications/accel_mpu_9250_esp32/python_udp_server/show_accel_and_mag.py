import show_accel
import show_mag
import pickle
from show_helper import start_show

mag_calib = pickle.load(open('mag.calib', 'rb'))
def mc():
    return mag_calib

accel_calib = pickle.load(open('accel.calib', 'rb'))
def ac():
    return accel_calib

def onReading(data):
    show_accel.onReading(data)
    show_mag.onReading(data)

ani_accel = show_accel.show_accel(ac, None, False)
ani_mag = show_mag.show_mag(mc, None, False)

start_show(onReading)