#!/bin/bash

#make

if [ -z "$1" ]; then
    echo -e "Usage: ./bench.sh <mounted filesystem>"
    exit
fi

mntdir=$(echo $1 | sed 's/\/$//')

bsarray=( 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000 2000000 5000000 )
countarray=( 1 5 10 50 100 500 1000 5000 10000 )

for i in "${bsarray[@]}"; do
    ./ddbench -b $i -c 1 $mntdir/benchfile >> bs_c1.txt
    rm $mntdir/benchfile
done

python3 graphe.py

