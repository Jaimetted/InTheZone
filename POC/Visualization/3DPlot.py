import matplotlib.pyplot as plt
from matplotlib import animation
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import serial
import threading

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# for each vector: x, y, z, u, v, w
V = [[0,0,0,0.5,0,0], [0,0,0,0,0.5,0], [0,0,0,0,0,0.5]]
Q = [ax.quiver(V[0][0], V[0][1], V[0][2], V[0][3], V[0][4], V[0][5], color="red"),
    ax.quiver(V[1][0], V[1][1], V[1][2], V[1][3], V[1][4], V[1][5], color="green"),
    ax.quiver(V[2][0], V[2][1], V[2][2], V[2][3], V[2][4], V[2][5], color="blue")]

ax.set_xlim([-1,1])
ax.set_ylim([-1,1])
ax.set_zlim([-1,1])
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
ax.scatter(0,0,0)

arduino = serial.Serial('/dev/cu.usbmodem1411', 9600, timeout=.1)
anglex = 0

def read_arduino(anglex):
    # get from arduino
    try:
        anglex = float(arduino.readline())
        print(anglex)
    except:
        anglex = anglex

def update_quivers(num, Q, V):
    Q[0].remove()
    Q[1].remove()
    Q[2].remove()
    Q[0] = ax.quiver(V[0][0], V[0][1], V[0][2], V[0][3], V[0][4], V[0][5], color="red")
    Q[1] = ax.quiver(V[1][0], V[1][1], V[1][2], V[1][3], V[1][4], V[1][5], color="green")
    Q[2] = ax.quiver(V[2][0], V[2][1], V[2][2], V[2][3], V[2][4], V[2][5], color="blue")

    return Q,

# you need to set blit=False, or the first set of arrows never gets
# cleared on subsequent frames
anim = animation.FuncAnimation(fig, update_quivers, fargs=(Q, V),
                               interval=50, blit=False)

thread = threading.Thread(target=read_arduino, args=(anglex,))
thread.start()

plt.show()
