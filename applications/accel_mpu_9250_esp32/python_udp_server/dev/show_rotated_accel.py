from numpy import genfromtxt
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3

data = genfromtxt('D:\\github\\arduino_JG\\applications\\accel_mpu_9250_esp32\\python_udp_server\\calib.csv', delimiter=',')

# Roll/Pitch/Yaw
rpy = {
        'ZDXR' : (0,0,0),   # Z down, X right
        'ZDYR' : (0,0,90),
        'ZDXL' : (0,0,180),
        'ZDYL' : (0,0,270),
        'XUZL' : (0,90,0),
        'XUYR' : (0,90,90),
        'XUZR' : (0,90,180),
        'XUYL' : (0,90,270),
        'ZUXL' : (0,180,0),
        'ZUYR' : (0,180,90),
        'ZUXR' : (0,180,180),
        'ZUYL' : (0,180,270),
        'XDZR' : (0,270,0),
        'XDYR' : (0,270,90),
        'XDZL' : (0,270,180),
        'XDYL' : (0,270,270),
        'YDXR' : (90,0,0),
        'YDZR' : (90,0,90),
        'YDXL' : (90,0,180),
        'YDZL' : (90,0,270),
        'YUXR' : (-90,0,0),
        'YUZL' : (-90,0,90),
        'YUXL' : (-90,0,180),
        'YUZR' : (-90,0,270),
        }

## Yaw Pitch Roll based on collection order
axes = ("X","Y","Z")
ud_s = ("U", "D")
lr_s = ("R", "L")

roll = np.empty((0,1))
pitch = np.empty((0,1))
yaw = np.empty((0,1))
i = -1
j = 0
while i < 34:
    vert = -1
    horz = -1
    while vert == horz:
        i += 1
        horz = int(i) % 3
        lr = int(i/3) % 2
        ud = int(i/6) % 2
        vert = int(i/12) % 3
    j += 1
    desc = axes[vert] + ud_s[ud] + axes[horz] + lr_s[lr]
    print(j, desc)
    RPY = rpy[desc]
    roll = np.concatenate((roll, RPY[0] * np.ones((200,1))))
    pitch = np.concatenate((pitch, RPY[1] * np.ones((200,1))))
    yaw = np.concatenate((yaw, RPY[2] * np  .ones((200,1))))

roll = np.pi * roll / 180
pitch = np.pi * pitch / 180
yaw = np.pi * yaw / 180

## Accelerometer
a_ = data[:,4:7]

a_max = np.max(a_, 0)
a_min = np.min(a_, 0)
a_scale = np.reshape((a_max - a_min) / 2, (1,3))
a_centre = np.reshape((a_max + a_min) / 2, (1,3))

# Normalise to limits
a = (a_ - np.matmul(np.ones((a_.shape[0],1)), a_centre)) / np.matmul(np.ones((a_.shape[0],1)), a_scale)

## Magnetometer
m_ = data[:,11:14]

m_max = np.max(m_, 0)
m_min = np.min(m_, 0)
m_scale = np.reshape((m_max - m_min) / 2, (1,3))
m_centre = np.reshape((m_max + m_min) / 2, (1,3))

# Normalise to limits
m = (m_ - np.matmul(np.ones((m_.shape[0],1)), m_centre)) / np.matmul(np.ones((m_.shape[0],1)), m_scale)
temp = np.copy(m[:,1])   # swap x and y
m[:,1] = m[:,0]
m[:,0] = temp
m[:,2] = -m[:,2]    # Magenetometer Z opposite to Accelerometer

xa = np.empty(a.shape)
xm = np.empty(a.shape)
for i in range(xa.shape[0]):
    Rx = np.array(( (1, 0, 0),
                    (0, np.cos(roll[i][0]), np.sin(roll[i][0])),
                    (0, -np.sin(roll[i][0]), np.cos(roll[i][0]))))
    Ry = np.array(( (np.cos(pitch[i][0]), 0, -np.sin(pitch[i][0])),
                    (0, 1, 0),
                    (np.sin(pitch[i][0]), 0, np.cos(pitch[i][0]))))
    Rz = np.array(( (np.cos(yaw[i][0]), np.sin(yaw[i][0]), 0),
                    (-np.sin(yaw[i][0]), np.cos(yaw[i][0]), 0),
                    (0, 0, 1)))
    R = np.matmul(Rx, np.matmul(Ry, Rz))

    xa[i,:] = np.matmul(np.linalg.inv(R), a[i,:])

    xm[i,:] = np.matmul(np.linalg.inv(R), m[i,:])

plt.figure(1)
plt.subplot(311)
plt.title("Rotated Accelerometer")
plt.plot(xa[:,0],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("X")
plt.grid()
plt.subplot(312)
plt.plot(xa[:,1],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Y")
plt.grid()
plt.subplot(313)
plt.plot(xa[:,2],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Z")
plt.grid()

plt.figure(2)
plt.subplot(311)
plt.title("Rotated Magnetometer")
plt.plot(xm[:,0],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("X")
plt.grid()
plt.subplot(312)
plt.plot(xm[:,1],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Y")
plt.grid()
plt.subplot(313)
plt.plot(xm[:,2],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Z")
plt.grid()

plt.figure(3)
plt.subplot(311)
plt.title("Raw Magnetometer")
plt.plot(m[:,0],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("X")
plt.grid()
plt.subplot(312)
plt.plot(m[:,1],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Y")
plt.grid()
plt.subplot(313)
plt.plot(m[:,2],'.');
plt.xticks(range(0,4800,200))
plt.ylabel("Z")
plt.grid()

plt.show()

# Note.
# To get this to work, the orientation of Z had to be changed.
# To have Z up, invert z measurements