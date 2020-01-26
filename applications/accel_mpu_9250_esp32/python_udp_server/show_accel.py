from accel_server import *
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3
import matplotlib.animation as animation
import threading
from mpl_toolkits.mplot3d import proj3d

def onReading(data):
    global ax, ay, az, _onReadingCallback

    ax.append(data.accel[0])
    ay.append(data.accel[1])
    az.append(data.accel[2])

    if _onReadingCallback:
        _onReadingCallback(data, ax, ay, az)

def update(num):
    global ax, ay, az, _accelCalib

    if len(ax) < 2:
        return  # Insufficient data

    ax_ = np.array(ax)
    ay_ = np.array(ay)
    az_ = np.array(az)

    if ax_.size != ay_.size or ax_.size != az_.size:
        return  # Out of sync

    accel_calib = _accelCalib()
    ax_ = (ax_ - accel_calib[0]) / accel_calib[1]
    ay_ = (ay_ - accel_calib[2]) / accel_calib[3]
    az_ = (az_ - accel_calib[4]) / accel_calib[5]

    (p_current, p_hist) = to_update
    p_hist.set_data(ax_, ay_)
    p_hist.set_3d_properties(az_)

    p_current.set_data((0, ax_[-1]), (0,ay_[-1]))
    p_current.set_3d_properties((0, az_[-1]))
    
    magnitude = np.sqrt(ax_[-1]*ax_[-1]+ay_[-1]*ay_[-1]+az_[-1]*az_[-1])
    ax_accel.set_title("%1.1f  %1.1f  %1.1f -> %1.1f"%(ax_[-1], ay_[-1], az_[-1], magnitude))
    return to_update

def show_accel(accelCalib, onReadingCallback = None, configFig = None):
    global _onReadingCallback, _accelCalib, ax, ay, az, to_update, ax_accel
    _onReadingCallback = onReadingCallback
    _accelCalib = accelCalib

    fig = plt.figure()
    ax_accel = p3.Axes3D(fig)
    ax_accel.plot([0,1],[0,0],[0,0],'r-')
    ax_accel.plot([0,0],[0,1],[0,0],'g-')
    ax_accel.plot([0,0],[0,0],[0,1],'b-')
    p_current, = ax_accel.plot([0,1],[0,1],[0,1],'k-o')
    p_hist, = ax_accel.plot([0,0.5],[0,0],[0,0],'y.')
    ax_accel.set_xlim3d([-1.2, 1.2])
    ax_accel.set_ylim3d([-1.2, 1.2])
    ax_accel.set_zlim3d([-1.2, 1.2])
    ax_accel.set_xlabel("X")
    ax_accel.set_ylabel("Y")
    ax_accel.set_zlabel("Z")
    ax_accel.set_title("TITLE")

    ax = []
    ay = []
    az = []

    to_update = (p_current, p_hist)
    line_ani = animation.FuncAnimation(fig, update, 3600,
                                    interval=10, blit=False)

    if configFig:
        configFig(fig)

    s = accel_server(onReading)

    thread = threading.Thread(target=lambda: s.run(), daemon=True)
    thread.start()

    plt.show()
    
def orthogonal_proj(zfront, zback):
    a = (zfront+zback)/(zfront-zback)
    b = -2*(zfront*zback)/(zfront-zback)
    return np.array([[1,0,0,0],
                        [0,1,0,0],
                        [0,0,a,b],
                        [0,0,0,zback]])
proj3d.persp_transformation = orthogonal_proj

if __name__ == '__main__':
    import pickle
    accel_calib = pickle.load(open('accel.calib', 'rb'))
    def ac():
        return accel_calib
    show_accel(ac)