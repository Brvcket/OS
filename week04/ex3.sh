#!/bin/bash

# Compile ex3.c
gcc ex3.c -o ex3

# Run the program with n=3
./ex3 3 &
echo "n=3"

# Run pstree multiple times
echo "1st pstree:"
pstree -p
sleep 3
echo "2nd pstree:"
pstree -p
sleep 5
echo "3rd pstree:"
pstree -p
sleep 3

# Run the program with n=5
./ex3 5 &
echo "n=5"

# Run pstree multiple times
echo "1st pstree:"
pstree -p
sleep 3
echo "2nd pstree:"
pstree -p
sleep 3
echo "3rd pstree:"
pstree -p
sleep 3
echo "4th pstree:"
pstree -p
sleep 3
echo "5th pstree:"
pstree -p
sleep 3
echo "6th pstree:"
pstree -p

# Explanation
echo "Explanation of results:" > ex3.txt
echo "When n = 3, we create 3 child processes. After 5 seconds, they sleep and become zombies. When n = 5, we create 5 child processes, and the similar behavior is happening, but we can see zombies from the previous program call before it's completed, then there is no process from the previous program call. To see this difference I added sleeps between pstree calls." >> ex3.txt
