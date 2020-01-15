import numpy as np
import matplotlib.pyplot as plt
import sys

xs = []
ys = []

for line in sys.stdin:
    *_, t, population = line.strip().split(" ")
    t, population = int(t), int(population)
    xs.append(t)
    ys.append(population)

plt.plot(xs, ys)
plt.show()
