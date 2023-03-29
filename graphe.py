import numpy as np
import matplotlib.pyplot as plt

filelist = {"bs_c1.txt"   : ["Blocksize [B]", "block size"], 
            "bs500_c.txt" : ["Nb of 500B block", "number of 500B block"],
            "bs_c_1g.txt" : ["Blocksize [B] / Nb of block", "combined blocksize and number of block to keep 1GB file"]}

for filename, labels in filelist.items():
    data = []
    with open(filename, "r") as file:
        rows = file.readlines()
        for row in rows:
            data.append(row.strip().split())

    data = np.array(data)
    head_array = data[::11,:] 
    lbs = len(head_array)

    data = np.array(np.array_split(data, lbs))
    mean_array = np.empty((lbs, 3))
    min_array = np.empty((lbs, 3))
    max_array = np.empty((lbs, 3))
    for i in range(lbs):
        temp = np.array(data[i,1:], dtype=float)      
        mean_array[i] = np.mean(temp, axis=0)
        min_array[i] = np.min(temp, axis=0)
        max_array[i] = np.max(temp, axis=0)

    time_yerr = np.concatenate((np.abs([mean_array[:,1] - min_array[:,1]]), np.abs([max_array[:,1] - mean_array[:,1]])), axis=0)
    bw_yerr   = np.concatenate((np.abs([mean_array[:,2] - min_array[:,2]]), np.abs([max_array[:,2] - mean_array[:,2]])), axis=0)
    
    fig = plt.figure(figsize=(15,12))
    if filename == "bs_c1.txt":
        plt.errorbar(head_array[:,1].astype(float), mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs500_c.txt":
        plt.errorbar(head_array[:,2].astype(float), mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs_c_1g.txt":
        plt.errorbar([str(i[1])+"/"+str(i[2]) for i in head_array], mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xticks(rotation=45)
    plt.xlabel(labels[0])
    plt.ylabel("Time [s]")
    plt.title("Writing time in function of "+labels[1])
    plt.savefig("time_"+filename[:-4]+".png")

    fig = plt.figure(figsize=(15,12))
    if filename == "bs_c1.txt":
        plt.errorbar(head_array[:,1].astype(float), mean_array[:,2], yerr=bw_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs500_c.txt":
        plt.errorbar(head_array[:,2].astype(float), mean_array[:,2], yerr=bw_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs_c_1g.txt":
        plt.errorbar([str(i[1])+"/"+str(i[2]) for i in head_array], mean_array[:,2], yerr=bw_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xticks(rotation=45)
    plt.xlabel(labels[0])
    plt.ylabel("Bandwidth[B/s]")
    plt.title("Writing bandwidth in function of "+labels[1])
    plt.savefig("bw_"+filename[:-4]+".png")


fig = plt.figure(figsize=(15,12))
plt.boxplot(data[:,1:,2].astype(float).transpose(), widths=0.5)
plt.xticks(np.arange(1, len(mean_array)+1), [str(i[1])+"/"+str(i[2]) for i in head_array], rotation=45)
plt.xlabel(labels[0])
plt.ylabel("Bandwidth[B/s]")
plt.title("Writing bandwidth in function of "+labels[1]+" (bloxplot)")
plt.savefig("bw_"+filename[:-4]+"_box.png")
