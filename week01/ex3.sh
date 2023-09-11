# Create root.txt and home.txt inside two separate new folders
# Before creating the next item, print the date and wait for 3 seconds
date
sleep 3
mkdir root
date
sleep 3
mkdir home
touch root.txt
date
sleep 3
touch home.txt
mv root.txt ~/week01/root
mv home.txt ~/week01/home
# ls the items from root and home directories sorted intp root.txt and home.txt files
ls / -t -r > ~/week01/root/root.txt
ls ~ -t -r > ~/week01/home/home.txt
# print the content of files and display items of the new folders 
cat < ~/week01/root/root.txt
cat < ~/week01/home/home.txt
ls ~/week01/home
ls ~/week01/root
