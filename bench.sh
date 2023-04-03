#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w)> <device>"
    exit
fi

filesize=$((1*1024*1024*1024))

## filesize=1G with count=2*1024*1024 to 64 and bs=512B to 16MB
for (( bs=512; bs<=16*1024*1024; bs=bs*2 )); do
    echo $bs
    ./mydd -m $1 -b $bs -c $((filesize/bs)) $2 >> bench_blockdev_$1.txt
done

python3 graph.py bench_blockdev_$1.txt

