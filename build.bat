rem clean
del *.o 
del run*
del test-*

rem compile

g++ -c iolib.cpp -o iolib.o
g++ -c getopt.cpp -o getopt.o

g++ -c dump.cpp -o dump.o
g++ -c cat.cpp -o cat.o
g++ -c find.cpp -o find.o

rem link

g++ iolib.o dump.o -o run-dump.exe
g++ iolib.o cat.o -o run-cat.exe
g++ getopt.o iolib.o find.o -o run-find.exe
