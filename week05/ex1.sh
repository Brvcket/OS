#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <max_subs_num>"
    exit 1
fi

gcc publisher.c -o publisher

for ((i = 1; i <= $1; i++)); do
    gcc subscriber.c -o subscriber$i
done

mkdir -p /tmp/ex1

for ((i = 1; i <= $1; i++)); do
    mkfifo /tmp/ex1/s$i
done

gnome-terminal --title="Publisher" -- bash -c "./publisher $1; exec bash"

for ((i = 1; i <= $1; i++)); do
    gnome-terminal --title="Subscriber $i" -- bash -c "./subscriber$i $i; exec bash"
done

echo "The publisher is running... Press <Enter> to stop it"
read

for ((i = 1; i <= $1; i++)); do
    rm -f /tmp/ex1/s$i
done

rmdir /tmp/ex1 2>/dev/null

rm -f publisher
for ((i = 1; i <= $1; i++)); do
    rm -f subscriber$i
done

