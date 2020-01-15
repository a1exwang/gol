import numpy as np
import matplotlib.pyplot as plt
import sys

ts = []
ps = []
es = []
energy_income = []

for line in sys.stdin:
    *_, t, population, energy, energy_income_s = line.strip().split(" ")
    ts.append(int(t))
    ps.append(int(population))
    es.append(int(energy))
    energy_income.append(int(energy_income_s))

plt.subplot("411")
plt.plot(ts, ps)
plt.title("Population")
plt.subplot("412")
plt.plot(ts, es)
plt.title("Energy")
plt.subplot("413")
plt.plot(ts, energy_income)
plt.title("Energy Income")
plt.show()
