# Basic Unix command dd benchmark

`myddlfs.c` is a basic C program that reads and writes to devices (sda, ram, ...) or file like Unix command dd`. Arguments are the mode (read or write), the block size in bytes, the number of block, if O_DIRECT or not and and the device or file (/dev/sda0, /dev/ram0, /mnt/sdaxfs/file0, /mnt/ramext4/file0, ...). It gives time in seconds and bandwith in bytes per second.
`bench.sh` repeats the execution of `myddlfs.c` with different block sizes and different number of block but with a same size of total read or written memory. For each repetition, bandwidth is sent to a txt file for further processing and the cache is cleared.
`graph.py` plots the graphs corresponding to the measurements taken previously.

Examples : 
```
make
./mydd file1 					#default : write, 512 bytes block, 1 block, 512B file in curent directory
./mydd -m w -b 1048576 -c 1024 file1		#write, 1MB block, 1024 blocks, 1G file in current directory
./mydd -m w -d -b 1048576 -c 1024 file1		#same but with O_DIRECT flag in order to avoid caching
./mydd -m r -b 1024 -c 8192 /dev/ram0		#read, 1KB block, 8192 blocks, 8MB read data in RAM0
```

Bench examples : Same logic by adding a "d" for direct IO
```
make
./bench.sh w sda_ext4 /mnt/sdaext4/file1
./bench.sh r sda_ext4 /mnt/sdaext4/file1
./bench.sh rd sda_ext4_direct /mnt/sdaext4/file1
python3 graph.py bench_sda_ext4_wr.txt
```
