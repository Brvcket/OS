#!/bin/bash

gcc worker.c -o worker
gcc scheduler_rr.c -o scheduler_rr

# Check if compilation was successful
if [ $? -eq 0 ]; then
    # Check if the quantum argument is provided
    if [ $# -eq 1 ]; then
        # Run the program with the provided quantum value
        ./scheduler_rr data.txt "$1"
    else
        echo "Usage: $0 <quantum>"
    fi
else
    echo "Compilation failed"
fi

rm worker
rm scheduler_rr
