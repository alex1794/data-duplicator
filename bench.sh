#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w or a)> <fs type> <device>"
    exit
fi

filesize=$((1*1024*1024*1024))

## filesize=1G with count=2*1024*1024 to 64 and bs=512B to 16MB

if [ "$1" = "w" ] || [ "$1" = "a" ]; then
    for (( bs=512; bs<=16*1024*1024; bs=bs*2 )); do
        echo -n "$bs "
        ./mydd -m w -b $bs -c $((filesize/bs)) $3 >> bench_$2_w.txt
        echo 3 | sudo tee /proc/sys/vm/drop_caches
    done
fi

if [ "$1" = "r" ] || [ "$1" = "a" ]; then
    for (( bs=512; bs<=16*1024*1024; bs=bs*2 )); do
        echo -n "$bs "
        ./mydd -m r -b $bs -c $((filesize/bs)) $3 >> bench_$2_r.txt
        echo 3 | sudo tee /proc/sys/vm/drop_caches
    done
fi

if [ "$1" = "a" ]; then
    cat bench_$2_w.txt >> bench_$2_wr.txt
    cat bench_$2_r.txt >> bench_$2_wr.txt
    rm bench_$2_w.txt bench_$2_r.txt
fi

