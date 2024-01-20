#UNAME_S=`uname -s`
LDFLAGS=
OBJS=

if [[ "$OSTYPE" == "darwin"* ]]; then
    LDFLAGS='-lobjc -framework Foundation -framework CoreFoundation -framework CoreServices'
    OBJS=sysos_mac.o
fi

# clean
./clean.sh

# compile
############
g++ -c strlib.cpp -o strlib.o
g++ -c wstrlib.cpp -o wstrlib.o
g++ -c pathlib.cpp -o pathlib.o
g++ -c wclib.cpp -o wclib.o
g++ -c fslib.cpp -o fslib.o
g++ -c iolib.cpp -o iolib.o
g++ -c srhlib.cpp -o srhlib.o
g++ -c sysos.cpp -o sysos.o
g++ -c syslocale.cpp -o syslocale.o
g++ -c syslocale_os.cpp -o syslocale_os.o
g++ -c syslib.cpp -o syslib.o
g++ -c getopt.cpp -o getopt.o
g++ -c test_getopt.cpp -o test_getopt.o
g++ -c run_dump.cpp -o run_dump.o
g++ -c run_cat.cpp -o run_cat.o
g++ -c run_find.cpp -o run_find.o

if [[ "$OSTYPE" == "darwin"* ]]; then
   g++ -c sysos_mac.mm -o sysos_mac.o
fi

# link
###########
g++ $LDFLAGS iolib.o run_dump.o -o run-dump
g++ $LDFLAGS iolib.o strlib.o wstrlib.o syslocale_os.o syslib.o run_cat.o -o run-cat
g++ $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o srhlib.o sysos.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find
g++ $LDFLAGS getopt.o test_getopt.o -o test-getopt
