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
    
    xlab1 = ["500B", "1K", "2", "5", "10", "20", "50", "100", "200", "500", "1M", "2", "5", "10", "20", "50", "100", "200", "500", "1G"]
    xlab2 = ["500B/2000000", "1K/1000000", "2K/500000", "5K/200000", "10K/100000", "20K/50000", "50K/20000", "100K/10000", "200K/5000", "500K/2000", "1M/1000", "2M/500", "5M/200"]

    fig = plt.figure(figsize=(15,12))
    if filename == "bs_c1.txt":
        plt.errorbar(head_array[:,1].astype(float), mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
        plt.xticks(head_array[:,1].astype(float), xlab1)
    elif filename == "bs500_c.txt":
        plt.errorbar(head_array[:,2].astype(float), mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs_c_1g.txt":
        plt.errorbar([str(i[1])+"/"+str(i[2]) for i in head_array], mean_array[:,1], yerr=time_yerr, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xticks([str(i[1])+"/"+str(i[2]) for i in head_array], xlab2, rotation=45)
    plt.xlabel(labels[0])
    plt.ylabel("Time [s]")
    plt.title("Writing time in function of "+labels[1])
    plt.savefig("time_"+filename[:-4]+".png")

    fig = plt.figure(figsize=(15,12))
    if filename == "bs_c1.txt":
        plt.errorbar(head_array[:,1].astype(float), mean_array[:,2]*1e-6, yerr=bw_yerr*1e-6, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
        plt.xticks(head_array[:,1].astype(float), xlab1)
    elif filename == "bs500_c.txt":
        plt.errorbar(head_array[:,2].astype(float), mean_array[:,2]*1e-6, yerr=bw_yerr*1e-6, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xscale('log')
    elif filename == "bs_c_1g.txt":
        plt.errorbar([str(i[1])+"/"+str(i[2]) for i in head_array], mean_array[:,2]*1e-6, yerr=bw_yerr*1e-6, ecolor="red", elinewidth=1.0, capsize=2.0, capthick=1.0)
        plt.xticks([str(i[1])+"/"+str(i[2]) for i in head_array], xlab2, rotation=45)
    plt.xlabel(labels[0])
    plt.ylabel("Bandwidth[MB/s]")
    plt.title("Writing bandwidth in function of "+labels[1])
    plt.savefig("bw_"+filename[:-4]+".png")


fig = plt.figure(figsize=(15,12))
plt.boxplot(data[:,1:,2].astype(float).transpose()*1e-6, widths=0.5)
plt.xticks(np.arange(1, len(mean_array)+1), xlab2, rotation=45)
plt.xlabel(labels[0])
plt.ylabel("Bandwidth[MB/s]")
plt.title("Writing bandwidth in function of "+labels[1]+" (boxplot)")
plt.savefig("bw_"+filename[:-4]+"_box.png")
