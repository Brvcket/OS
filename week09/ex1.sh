#!/bin/bash

# example: ./ex1.sh 4 2 random R0 R1 W1 R0 R2 W2 R0 R3 W2
pages=$1
frames=$2
algo=$3
ref_string=${@:4}

gcc mmu.c -o mmu
gcc pager.c -o pager

mkdir -p /tmp/ex2/
touch /tmp/ex2/pagetable.txt

./pager $pages $frames $algo > pager.txt &

pagerPID=$(pidof -s pager)
echo "Pager PID: $pagerPID"

./mmu $pages "$ref_string" "$pagerPID" > mmu.txt

rm -rf /tmp/ex2/
rm mmu
rm pager


