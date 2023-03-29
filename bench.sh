#!/bin/bash

#make

if [ -z "$1" ]; then
    echo -e "Usage: ./bench.sh <mounted filesystem>"
    exit
fi

mntdir=$(echo $1 | sed 's/\/$//')

bsarray=( 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000 2000000 5000000 )
countarray=( 1 5 10 50 100 500 1000 5000 10000 50000 100000 500000 1000000 2000000 )
gigarray=( 2000000 1000000 500000 200000 100000 50000 20000 10000 5000 2000 1000 500 200 )

## count=1 and bs=500B to 5M
for i in "${bsarray[@]}"; do
    ./ddbench -b $i -c 1 $mntdir/benchfile >> bs_c1.txt
    rm $mntdir/benchfile
done

## count=1 to 2000000 and bs=500B
for i in "${countarray[@]}"; do
    ./ddbench -b 500 -c $i $mntdir/benchfile >> bs500_c.txt
    rm $mntdir/benchfile
done

## filesize=1G with count=2000000 to 200 and bs=500B to 5M
for (( i=0; i<13; i++ )); do
    echo $i
    ./ddbench -b ${bsarray[$i]} -c ${gigarray[$i]} $mntdir/benchfile >> bs_c_1g.txt
    rm $mntdor/benchfile
done

python3 graphe.py

