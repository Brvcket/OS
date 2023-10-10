gcc -pthread ex2.c -o ex2

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <threads_num>"
    exit 1
fi

./ex2 $1

rm -f ex2
