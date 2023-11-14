#!/bin/bash

gcc monitor.c -o monitor
gcc ex1.c -o ex1

if [ ! -d $1 ]; then
    mkdir $1
fi

gnome-terminal -- ./monitor $1
./ex1 $1