#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w or a)> <fs type> <device/file>"
    exit
fi

filesize=$((4*1024*1024*1024))
mode=$1
direct=""
bsmin=4096
bsmax=$((2*1024*1024*1024))
if [ "$1" = "wd" ] || [ "$1" = "rd" ]; then
    echo -n "DIRECT "
    direct="-d"
fi

## filesize=1G with count=2*1024*1024 to 64 and bs=512B to 16MB

if [ "${mode:0:1}" = "w" ] || [ "${mode:0:1}" = "a" ]; then
    echo "WRITING"
    for (( bs=bsmin; bs<=bsmax; bs=bs*2 )); do
        echo -n "$bs "
        ./mydd -m w -b $bs -c $((filesize/bs)) $direct $3 | tee -a bench_$2_w.txt
        echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
    done
fi

if [ "${mode:0:1}" = "r" ] || [ "${mode:0:1}" = "a" ]; then
    echo "READING"
    for (( bs=bsmin; bs<=bsmax; bs=bs*2 )); do
        echo -n "$bs "
        ./mydd -m r -b $bs -c $((filesize/bs)) $direct $3 | tee -a bench_$2_r.txt
        echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
    done
fi

if [ "${mode:0:1}" = "a" ]; then
    cat bench_$2_w.txt >> bench_$2_wr.txt
    cat bench_$2_r.txt >> bench_$2_wr.txt
    rm bench_$2_w.txt bench_$2_r.txt
fi

