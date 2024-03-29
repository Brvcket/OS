#!/bin/bash

# example: ./ex2.sh 4 2 R0 R1 W1 R0 R2 W2 R0 R3 W2 (added clarification)

pages=$1
frames=$2
ref_string=${@:3}

gcc mmu.c -o mmu
gcc pager.c -o pager

mkdir -p /tmp/ex2/
touch /tmp/ex2/pagetable.txt

./pager $pages $frames > pager.txt &

pagerPID=$(pidof -s pager)
echo "Pager PID: $pagerPID"

./mmu 4 "$ref_string" "$pagerPID" > mmu.txt

rm -rf /tmp/ex2/
rm mmu
rm pager


echo "Outputs of programs are in pager.txt and mmu.txt"
