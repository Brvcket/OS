#!/bin/bash

gcc -o ex1 ex1.c 
./ex1 &

pid=$(cat /tmp/ex1.pid)

str=$(grep heap /proc/$pid/maps)

str=$(sudo xxd -s "0x${str:0:12}" -l $(("0x${str:13:12}-0x${str:0:12}")) /proc/$pid/mem | grep pass)
pass="${str#*:}"
pass="${pass#*:}"

if [[ -z $pass ]]; then
    echo "Error while reading the password("
    sudo kill -9 $pid
    exit 1
fi

echo "Password was found at address: 0x${str:0:12}"
echo "Password: $pass"

sudo kill -9 $pid
rm ex1
