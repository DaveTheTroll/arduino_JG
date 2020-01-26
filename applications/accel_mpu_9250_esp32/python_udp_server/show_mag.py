from accel_server import *
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3
import matplotlib.animation as animation
import threading
from mpl_toolkits.mplot3d import proj3d

def onReading(data):
    global mx, my, mz, _onReadingCallback

    mx.append(data.mag[0])
    my.append(data.mag[1])
    mz.append(-data.mag[2])

    if _onReadingCallback:
        _onReadingCallback(data, mx, my, mz)

def update(num):
    global mx, my, mz, _magCalib

    if len(mx) < 2:
        return  # Insufficient data

    mx_ = np.array(mx)
    my_ = np.array(my)
    mz_ = np.array(mz)

    if mx_.size != my_.size or mx_.size != mz_.size:
        return  # Out of sync

    mag_calib = _magCalib()
    mx_ = (mx_ - mag_calib[0]) / mag_calib[1]
    my_ = (my_ - mag_calib[2]) / mag_calib[3]
    mz_ = (mz_ - mag_calib[4]) / mag_calib[5]

    (p_current, p_hist) = to_update
    p_hist.set_data(mx_, my_)
    p_hist.set_3d_properties(mz_)

    p_current.set_data((0, mx_[-1]), (0,my_[-1]))
    p_current.set_3d_properties((0, mz_[-1]))
    
    magnitude = np.sqrt(mx_[-1]*mx_[-1]+my_[-1]*my_[-1]+mz_[-1]*mz_[-1])
    ax.set_title("%1.1f  %1.1f  %1.1f -> %1.1f"%(mx_[-1], my_[-1], mz_[-1], magnitude))
    return to_update

def show_mag(magCalib, onReadingCallback = None):
    global _onReadingCallback, _magCalib, mx, my, mz, to_update, ax
    _onReadingCallback = onReadingCallback
    _magCalib = magCalib

    fig = plt.figure()
    ax = p3.Axes3D(fig)
    ax.plot([0,1],[0,0],[0,0],'r-')
    ax.plot([0,0],[0,1],[0,0],'g-')
    ax.plot([0,0],[0,0],[0,1],'b-')
    p_current, = ax.plot([0,1],[0,1],[0,1],'k-o')
    p_hist, = ax.plot([0,0.5],[0,0],[0,0],'y.')
    ax.set_xlim3d([-1.2, 1.2])
    ax.set_ylim3d([-1.2, 1.2])
    ax.set_zlim3d([-1.2, 1.2])
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title("TITLE")

    mx = []
    my = []
    mz = []

    to_update = (p_current, p_hist)
    line_ani = animation.FuncAnimation(fig, update, 3600,
                                    interval=10, blit=False)

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
    mag_calib = pickle.load(open('mag.calib', 'rb'))
    def mc():
        return mag_calib
    show_mag(mc)