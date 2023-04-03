#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w)> <device>"
    exit
fi

filesize=$((1*1024*1024*1024))

## filesize=1G with count=2000000 to 200 and bs=500B to 5M
for (( bs=512; bs<16*1024*1024; bs=bs*2 )); do
    ./ddbench -m $1 -b $bs -c $((filesize/bs)) $2
done

#python3 graphe.py

