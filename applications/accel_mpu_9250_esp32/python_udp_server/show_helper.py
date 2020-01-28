import threading
from mpl_toolkits.mplot3d import proj3d
import numpy as np
from accel_server import accel_server
import matplotlib.pyplot as plt

def orthogonal_proj(zfront, zback):
    a = (zfront+zback)/(zfront-zback)
    b = -2*(zfront*zback)/(zfront-zback)
    return np.array([[1,0,0,0],
                        [0,1,0,0],
                        [0,0,a,b],
                        [0,0,0,zback]])

def start_show(onReading):
    s = accel_server(onReading)

    thread = threading.Thread(target=lambda: s.run(), daemon=True)
    thread.start()
    
    proj3d.persp_transformation = orthogonal_proj
    plt.show()