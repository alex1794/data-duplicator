import numpy as np
import matplotlib.pyplot as plt

data = []
with open("bs_c1.txt", "r") as file:
    rows = file.readlines()
    for row in rows:
        data.append(row.strip().split())

data = np.array(data)
head_c1 = data[::11,:] 
lbs = len(head_c1)

data = np.array(np.array_split(data, lbs))
mean_c1 = np.empty((lbs, 3))
for i in range(lbs):
    temp = np.array(data[i,1:], dtype=float)
    mean_c1[i] = np.mean(temp, axis=0)

fig = plt.figure()
plt.plot(mean_c1[:,0], mean_c1[:,1])
plt.xscale('log')
plt.xlabel("Blocksize [B]")
plt.ylabel("Time [s]")
plt.title("Writing time in function of block size")
plt.savefig("time_bsc1.png")

fig = plt.figure()
plt.plot(mean_c1[:,0], mean_c1[:,2])
plt.xscale('log')
plt.xlabel("Blocksize [B]")
plt.ylabel("Bandwidth[B/s]")
plt.title("Writing bandwidth in function of block size")
plt.savefig("bw_bsc1.png")
