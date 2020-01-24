from accel_server import *
import matplotlib.pyplot as plt
import threading
from datetime import datetime
import numpy as np

lastDisplay = datetime.now()
missing = 0

def onReading(data):
    global mx, my, mz, lastDisplay, missing
    now = datetime.now()

    mx.append(data.mag[0])
    my.append(data.mag[1])
    mz.append(data.mag[2])

    # Only print and plot if not lagging
    if (now - lastDisplay).total_seconds() > 0.2:
        print("{%d} A [%6.0f %6.0f %6.0f] G [%6.0f %6.0f %6.0f] T %6d M [%6.0f %6.0f %6.0f] t %d |A| %6.0f |M| %6.0f"%(
            missing,
            data.accel[0], data.accel[1], data.accel[2],
            data.gyro[0], data.gyro[1], data.gyro[2],
            data.temperature,
            data.mag[0], data.mag[1], data.mag[2],
            data.timestamp,
            hypotn(data.accel),
            hypotn(data.mag)
            ))
        missing = 0

        while len(mx) > 1000:
            mx = mx[1:]
            my = my[1:]
            mz = mz[1:]

        pmx.set_xdata(range(len(mx)))
        pmx.set_ydata(mx.copy())
        pmy.set_xdata(range(len(my)))
        pmy.set_ydata(my.copy())
        pmz.set_xdata(range(len(mz)))
        pmz.set_ydata(mz.copy())

        mx_ = np.array(mx)
        my_ = np.array(my)
        mz_ = np.array(mz)
        magnitude = np.sqrt(mx_*mx_ + my_*my+ + mz_*mz_)

        pmm.set_xdata(range(len(mz)))
        pmm.set_ydata(magnitude)

        magnitude_max = max(magnitude)
        if magnitude_max > ax.get_ylim()[1]:
            ax.set_ylim((-magnitude_max, magnitude_max))

        plt.draw()
        lastDisplay = datetime.now()
    else:
        missing += 1

plt.figure(1)
pmx, pmy, pmz, pmm = plt.plot((0,1000), (-10, 10), '.', (0,1000), (-10, 10), '.', (0,1000), (-10, 10), '.', (0,1000), (-10, 10), '.')
ax = plt.gca()
plt.legend(('x','y','z'))
plt.grid()

mx = []
my = []
mz = []

s = accel_server(onReading)

thread = threading.Thread(target=lambda: s.run(), daemon=True)
thread.start()

plt.show()
