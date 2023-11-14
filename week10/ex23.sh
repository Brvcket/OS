#!/bin/bash

bash gen.sh 10 ex1.txt

ln ex1.txt ex11.txt
ln ex1.txt ex12.txt

cat ex1.txt
cat ex11.txt
cat ex12.txt

#or

#diff ex1.txt ex11.txt
#diff ex1.txt ex12.txt

#we see that content is the same

ls -i ex1.txt ex11.txt ex12.txt > output.txt

#2496635 ex11.txt
#2496635 ex12.txt
#2496635 ex1.txt

#As we see, inodes are the same, it happens because hard link is something like pointer to the file, so actually they are 3 names of the same file

du ex1.txt
#4       ex1.txt

ln ex1.txt ex13.txt
mv ex13.txt /tmp

find ./ -inum $(ls -i ex1.txt | cut -d' ' -f1)
#./ex11.txt
#./ex1.txt
#./ex12.txt


find / -inum $(ls -i ex1.txt | cut -d' ' -f1) 2>/dev/null
#/tmp/ex13.txt


#/home/bulatik/OS/week10/ex11.txt
#/home/bulatik/OS/week10/ex1.txt
#/home/bulatik/OS/week10/ex12.txt


#The difference between searching in the current path and the root path is the scope
#When searching in the current path (./), it only looks within the current directory and its subdirectories. Searching from the root path (/) scans the entire filesystem, starting from the root directory.
#In this case, difference is that for root path we see also ex13.txt

ls -l ex1.txt
# 4

find / -inum $(ls -i ex1.txt | cut -d' ' -f1) -exec rm {} \;

