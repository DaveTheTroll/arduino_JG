from accel_server import *
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3
import matplotlib.animation as animation
import threading

def onReading(data):
    global mx, my, mz

    mx.append(data.mag[0])
    my.append(data.mag[1])
    mz.append(data.mag[2])

def update(num):
    global mx, my, mz

    if len(mx) < 2:
        return  # Insufficient data

    mx_ = np.array(mx)
    my_ = np.array(my)
    mz_ = np.array(mz)

    if mx_.size != my_.size or mx_.size != mz_.size:
        return  # Out of sync

    mx_max = np.max(mx_)
    mx_min = np.min(mx_)
    mx_0 = (mx_max + mx_min)/2
    mx_s = (mx_max - mx_min)/2
    mx_ = (mx - mx_0) / mx_s
    
    my_max = np.max(my_)
    my_min = np.min(my_)
    my_0 = (my_max + my_min)/2
    my_s = (my_max - my_min)/2
    my_ = (my - my_0) / my_s
    
    mz_max = np.max(mz_)
    mz_min = np.min(mz_)
    mz_0 = (mz_max + mz_min)/2
    mz_s = (mz_max - mz_min)/2
    mz_ = (mz - mz_0) / mz_s

    p_hist.set_data(mx_, my_)
    p_hist.set_3d_properties(mz_)

    p_current.set_data((0, mx_[-1]), (0,my_[-1]))
    p_current.set_3d_properties((0, mz_[-1]))

    print(("%1.3f %1.3f %1.3f"%(mx_[-1], my_[-1], mz_[-1])))
    return to_update

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