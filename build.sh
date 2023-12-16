# clean
rm *.o 
rm run*

# compile
############
g++ -c iolib.cpp -o iolib.o

g++ -c dump.cpp -o dump.o
g++ -c cat.cpp -o cat.o
g++ -c find.cpp -o find.o

# link
###########
g++ iolib.o dump.o -o run-dump
g++ iolib.o cat.o -o run-cat
g++ iolib.o find.o -o run-find
