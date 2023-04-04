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

filename = sys.argv[1]
mode, bs, count, filesize, time, bw = dataread(filename)

marray = ["read", "write"]

fig = plt.figure(figsize=(15,12))
plt.plot(bs[mode==1].astype(str), bw[mode==1]/(1024**2), marker='+', markeredgecolor='r', markersize=12, label=marray[1])
plt.plot(bs[mode==0].astype(str), bw[mode==0]/(1024**2), marker='+', markeredgecolor='r', markersize=12, label=marray[0])
#plt.yscale("log")
if filename[6:9] == "ram":
    plt.ylim(0,2550)
else:
    plt.ylim(0,200)
plt.xlabel("Blocksize [B]")
plt.ylabel("Bandwidth [MB/s]")
plt.title("Bandwidth as a function of blocksize ("+filename[6:-4]+")")
plt.legend(loc='best')
plt.savefig("bw_"+filename[6:-4]+".png")
