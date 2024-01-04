rem clean
del *.o 
del run*
del test-*

rem compile

g++ -c strlib.cpp -o strlib.o
g++ -c pathlib.cpp -o pathlib.o
g++ -c fslib.cpp -o fslib.o
g++ -c iolib.cpp -o iolib.o
g++ -c getopt.cpp -o getopt.o

g++ -c dump.cpp -o dump.o
g++ -c cat.cpp -o cat.o
g++ -c find.cpp -o find.o

rem link

g++ iolib.o dump.o -o run-dump.exe
g++ iolib.o cat.o -o run-cat.exe
g++ getopt.o strlib.o pathlib.o fslib.o iolib.o find.o -o run-find.exe
