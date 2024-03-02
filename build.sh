CC=g++
#UNAME_S=`uname -s`
CFLAGS='-c -I./lib'
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
$CC $CFLAGS lib/strlib.cpp -o strlib.o
$CC $CFLAGS lib/wstrlib.cpp -o wstrlib.o

$CC $CFLAGS pathlib.cpp -o pathlib.o
$CC $CFLAGS wclib.cpp -o wclib.o
$CC $CFLAGS fslib.cpp -o fslib.o
$CC $CFLAGS iolib.cpp -o iolib.o
$CC $CFLAGS srhlib.cpp -o srhlib.o
$CC $CFLAGS syscpu.cpp -o syscpu.o
$CC $CFLAGS sysos.cpp -o sysos.o
$CC $CFLAGS sysuser.cpp -o sysuser.o
$CC $CFLAGS syslocale.cpp -o syslocale.o
$CC $CFLAGS syslocale_os.cpp -o syslocale_os.o
$CC $CFLAGS sysinfo.cpp -o sysinfo.o
$CC $CFLAGS syslib.cpp -o syslib.o
$CC $CFLAGS getopt.cpp -o getopt.o
$CC $CFLAGS test_getopt.cpp -o test_getopt.o

$CC $CFLAGS run_dump.cpp -o run_dump.o
$CC $CFLAGS run_echo.cpp -o run_echo.o
$CC $CFLAGS run_cat.cpp -o run_cat.o
$CC $CFLAGS run_sysinfo.cpp -o run_sysinfo.o
$CC $CFLAGS run_find.cpp -o run_find.o
$CC $CFLAGS run_ls.cpp -o run_ls.o

$CC $CFLAGS run_mkdir.cpp -o run_mkdir.o
$CC $CFLAGS run_rmdir.cpp -o run_rmdir.o

if [[ "$OSTYPE" == "darwin"* ]]; then
   $CC $CFLAGS sysos_mac.mm -o sysos_mac.o
fi

# link
###########
$CC $LDFLAGS getopt.o iolib.o run_dump.o -o run-dump
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_echo.o -o run-echo
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o srhlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_find.o -o run-find
$CC $LDFLAGS getopt.o test_getopt.o -o test-getopt

$CC $LDFLAGS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o run_ls.o -o run-ls

$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir

