# clean
rm *.o 
rm run*
rm test-*

# compile
############
g++ -c iolib.cpp -o iolib.o
g++ -c getopt.cpp -o getopt.o
#g++ -c test_getopt.cpp -o test_getopt.o

g++ -c dump.cpp -o dump.o
g++ -c cat.cpp -o cat.o
g++ -c find.cpp -o find.o

# link
###########
g++ iolib.o dump.o -o run-dump
g++ iolib.o cat.o -o run-cat
g++ getopt.cpp iolib.o find.o -o run-find

#g++ getopt.o test_getopt.o -o test-getopt
#g++ test_getopt.o -o test-getopt

