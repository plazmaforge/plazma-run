rem clean
del *.o 
del run-*
del test-*

rem compile

g++ -c strlib.cpp -o strlib.o
g++ -c wstrlib.cpp -o wstrlib.o
g++ -c pathlib.cpp -o pathlib.o
g++ -c fslib.cpp -o fslib.o
g++ -c wclib.cpp -o wclib.o
g++ -c iolib.cpp -o iolib.o
g++ -c srhlib.cpp -o srhlib.o
g++ -c syslocale.cpp -o syslocale.o
g++ -c syslocale_os.cpp -o syslocale_os.o
g++ -c syslib.cpp -o syslib.o
g++ -c getopt.cpp -o getopt.o

g++ -c run_dump.cpp -o run_dump.o
g++ -c run_cat.cpp -o run_cat.o
g++ -c run_find.cpp -o run_find.o

rem link

g++ iolib.o run_dump.o -o run-dump.exe
g++ iolib.o strlib.o wstrlib.o run_cat.o -o run-cat.exe
g++ getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o srhlib.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find.exe
