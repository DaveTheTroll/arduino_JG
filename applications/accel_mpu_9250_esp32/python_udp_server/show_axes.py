
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d import proj3d

class Arrow3D(FancyArrowPatch):
    def __init__(self, xs, ys, zs, *args, **kwargs):
        FancyArrowPatch.__init__(self, (0,0), (0,0), *args, **kwargs)
        self._verts3d = xs, ys, zs

    def draw(self, renderer):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, renderer.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))
        FancyArrowPatch.draw(self, renderer)

fig = plt.figure(1)
ax = fig.add_subplot(111, projection='3d')

ax.plot([0, 1], [0, 0], [0, 0], 'r-')
ax.plot([0, 0], [0, 1], [0, 0], 'g-')
ax.plot([0, 0], [0, 0], [0, 1], 'b-')
ax._axis3don = False

for a in range(360):
    ax.view_init(30, a)
    plt.draw()
    plt.pause(0.001)