# sudo apt install libxcb-cursor0
# python3 -m venv .venv
# source .venv/bin/activate
# pip install laspy numpy matplot PyQt6

import laspy
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

# reding general info
las = laspy.read("../build/test.las")
print("header", las.header)
print("point_count", las.header.point_count)
print("scales", las.header.scales)
print("offsets", las.header.offsets)

point_format = las.point_format
print(list(point_format.dimension_names))

# copy points
point_records = las.points.copy()

# getting scaling and offset parameters
las_scaleX = las.header.scale[0]
las_offsetX = las.header.offset[0]
las_scaleY = las.header.scale[1]
las_offsetY = las.header.offset[1]
las_scaleZ = las.header.scale[2]
las_offsetZ = las.header.offset[2]

# calculating coordinates
p_X = np.array((point_records['X'] * las_scaleX) + las_offsetX)
p_Y = np.array((point_records['Y'] * las_scaleY) + las_offsetY)
p_Z = np.array((point_records['Z'] * las_scaleZ) + las_offsetZ)

# plotting points
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(p_X, p_Y, p_Z, marker='.')
plt.show()
