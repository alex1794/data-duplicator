#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w)> <fs type> <device>"
    exit
fi

filesize=$((1*1024*1024*1024))

## filesize=1G with count=2*1024*1024 to 64 and bs=512B to 16MB
for (( bs=512; bs<=16*1024*1024; bs=bs*2 )); do
    echo -n "$bs "
    ./mydd -m $1 -b $bs -c $((filesize/bs)) $3 >> bench_$2_$1.txt
    echo 3 | sudo tee /proc/sys/vm/drop_caches
done

