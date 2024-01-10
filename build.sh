# clean
rm *.o 
rm run*
rm test-*

# compile
############
g++ -c strlib.cpp -o strlib.o
g++ -c wstrlib.cpp -o wstrlib.o
g++ -c pathlib.cpp -o pathlib.o
g++ -c wclib.cpp -o wclib.o
g++ -c fslib.cpp -o fslib.o
g++ -c iolib.cpp -o iolib.o
g++ -c srhlib.cpp -o srhlib.o
g++ -c syslib.cpp -o syslib.o
g++ -c getopt.cpp -o getopt.o
g++ -c test_getopt.cpp -o test_getopt.o

g++ -c dump.cpp -o dump.o
g++ -c cat.cpp -o cat.o
g++ -c find.cpp -o find.o

# link
###########
g++ iolib.o dump.o -o run-dump
g++ iolib.o strlib.o wstrlib.o cat.o -o run-cat
g++ getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o srhlib.o syslib.o find.o -o run-find

g++ getopt.o test_getopt.o -o test-getopt
#g++ test_getopt.o -o test-getopt

