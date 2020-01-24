import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3
import matplotlib.animation as animation
from matplotlib.widgets import Slider

def update_lines(num, lines):
    ϕ = roll * np.pi / 180   # roll
    θ = pitch * np.pi / 180   # pitch
    ψ = yaw * np.pi / 180   # yaw
    Rx = np.array([[1,0,0],[0,np.cos(ϕ),np.sin(ϕ)],[0,-np.sin(ϕ),np.cos(ϕ)]])
    Ry = np.array([[np.cos(θ),0,-np.sin(θ)],[0,1,0],[np.sin(θ),0,np.cos(θ)]])
    Rz = np.array([[np.cos(ψ),np.sin(ψ),0],[-np.sin(ψ),np.cos(ψ),0],[0,0,1]])
    R = np.matmul(Rz,np.matmul(Ry,Rx))
    
    x_ = np.array([1,0,0])
    x = np.matmul(R, x_)
    px.set_data([0, x[0]], [0, x[1]])
    px.set_3d_properties([0, x[2]])
    
    y_ = np.array([0,1,0])
    y = np.matmul(R, y_)
    py.set_data([0, y[0]], [0, y[1]])
    py.set_3d_properties([0, y[2]])
    
    z_ = np.array([0,0,1])
    z = np.matmul(R, z_)
    pz.set_data([0, z[0]], [0, z[1]])
    pz.set_3d_properties([0, z[2]])
    return lines

# Attaching 3D axis to the figure
fig = plt.figure()
ax = p3.Axes3D(fig)

ax.plot([0,0.5],[0,0],[0,0],'k-',linewidth=3)
ax.plot([0,0],[0,0.5],[0,0],'k-',linewidth=3)
ax.plot([0,0],[0,0],[0,0.5],'k-',linewidth=3)
px, = ax.plot([0,1],[0,0],[0,0],'r-')
py, = ax.plot([0,0],[0,1],[0,0],'g-')
pz, = ax.plot([0,0],[0,0],[0,1],'b-')

# Setting the axes properties
ax.set_xlim3d([-1.0, 1.0])
ax.set_ylim3d([-1.0, 1.0])
ax.set_zlim3d([-1.0, 1.0])
ax._axis3don = False

ax_roll = plt.axes([0.25, 0.1, 0.65, 0.03], facecolor='red')
ax_pitch = plt.axes([0.25, 0.15, 0.65, 0.03], facecolor='green')
ax_yaw = plt.axes([0.25, 0.2, 0.65, 0.03], facecolor='blue')

s_roll = Slider(ax_roll, 'Roll', 0, 360, valinit=0, valstep=45)
s_pitch = Slider(ax_pitch, 'Pitch', 0, 360, valinit=0, valstep=45)
s_yaw = Slider(ax_yaw, 'Yaw', 0, 360, valinit=0, valstep=45)

def update(val):
    global roll, pitch, yaw
    (roll, pitch, yaw) = (s_roll.val, s_pitch.val, s_yaw.val)
    print("%3d %3d %3d"%(roll, pitch, yaw))

s_roll.on_changed(update)
s_pitch.on_changed(update)
s_yaw.on_changed(update)
update(-1)
# Creating the Animation object
lines = (px, py, pz)
line_ani = animation.FuncAnimation(fig, update_lines, 360, fargs=(lines, ),
                                   interval=50, blit=True)

plt.show()
