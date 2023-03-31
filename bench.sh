#!/bin/bash

#make

if [ -z "$1" ] || [ -z "$2" ]; then
    echo -e "Usage: ./bench.sh <mode(r or w)> <mounted filesystem>"
    exit
fi

mntdir=$(echo $2 | sed 's/\/$//')

bsarray=( 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000 2000000 5000000 10000000 20000000 50000000 100000000 )
countarray=( 1 5 10 50 100 500 1000 5000 10000 50000 100000 500000 1000000 2000000 )
gigarray=( 2000000 1000000 500000 200000 100000 50000 20000 10000 5000 2000 1000 500 200 )

if [ "$1" = "r" ]; then
    echo "READING TESTS"
    sarray=( 1 2 5 )
    narray=( 10000 100000 1000000 10000000 100000000 1000000000 )
    
    if [ ! -f "$mntdir/file_4" ]; then
        echo "Test files don't exist, creating test files..."
        ./mydd -b 4000 -c 1 $mntdir/file_4
        for i in "${narray[@]}"; do
            for j in "${sarray[@]}"; do
                echo
                temp="$(echo "$i*$j" | bc)"
                ./mydd -b $temp -c 1 $mntdir/file_${temp:0: -3}
            done
        done
        echo
        rm $mntdir/file_5000000
        ./mydd -b 5000000 -c 1000 $mntdir/file_5000000
        echo
        ./mydd -b 10000000 -c 1000 $mntdir/file_10000000
        echo "Test files created"
    fi

    lsmnt=( $(ls $mntdir/file*) )

    for file in "${lsmnt[@]}"; do
        echo $file
        for i in "${bsarray[@]}"; do
            echo $i
            ./ddbench -m r -b $i $file >> bs_${file:11}.txt
        done
    done
elif [ "$1" = "w" ]; then
    echo "WRITING TESTS"

    ## count=1 and bs=500B to 5M
    for i in "${bsarray[@]}"; do
        echo $i
        ./ddbench -m $1 -b $i -c 1 $mntdir/benchfile >> bs_c1.txt
        rm $mntdir/benchfile
    done

    ## count=1 to 2000000 and bs=500B
    for i in "${countarray[@]}"; do
        echo $i
        ./ddbench -m $1 -b 500 -c $i $mntdir/benchfile >> bs500_c.txt
        rm $mntdir/benchfile
    done

    ## filesize=1G with count=2000000 to 200 and bs=500B to 5M
    for (( i=0; i<13; i++ )); do
        echo $i
        ./ddbench -m $1 -b ${bsarray[$i]} -c ${gigarray[$i]} $mntdir/benchfile >> bs_c_1g.txt
        rm $mntdir/benchfile
    done
else
    echo "Bad mode (only r or w)"
fi

#python3 graphe.py

