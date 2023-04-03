import numpy as np
import matplotlib.pyplot as plt
import sys

def dataread(filename):
    data = []
    with open(filename, "r") as file:
        rows = file.readlines()
        for row in rows:
            data.append(row.strip().split())

    data = np.array(data, dtype=float)
    return data[:,0].astype(int), data[:,1].astype(int), data[:,2].astype(int), data[:,3].astype(int), data[:,4], data[:,5]

mode, bs, count, filesize, time, bw = dataread(sys.argv[1])

marray = ["read", "write"]

fig = plt.figure(figsize=(15,12))
plt.plot(bs.astype(str), bw/(1024**2), marker='+', markeredgecolor='r', markersize=12, label=marray[mode[0]])
plt.xlabel("Blocksize [B]")
plt.ylabel("Bandwidth [MB/s]")
plt.title("Bandwidth as a function of blocksize")
plt.legend(loc='best')
plt.savefig("bw_bs.png")
