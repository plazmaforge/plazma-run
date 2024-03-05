set "CC=g++"
set "CFLAGS=-c -Ilib"
set "LDFLAGS="
set "LIBS=-lversion"

rem clean
call clean.bat

rem compile

%CC% %CFLAGS% lib/strlib.cpp -o strlib.o
%CC% %CFLAGS% lib/wstrlib.cpp -o wstrlib.o
%CC% %CFLAGS% lib/pathlib.cpp -o pathlib.o
%CC% %CFLAGS% lib/fslib.cpp -o fslib.o
%CC% %CFLAGS% lib/wclib.cpp -o wclib.o
%CC% %CFLAGS% lib/iolib.cpp -o iolib.o
%CC% %CFLAGS% lib/asklib.cpp -o asklib.o
%CC% %CFLAGS% lib/syscpu.cpp -o syscpu.o
%CC% %CFLAGS% lib/sysos.cpp -o sysos.o
%CC% %CFLAGS% lib/sysuser.cpp -o sysuser.o
%CC% %CFLAGS% lib/syslocale.cpp -o syslocale.o
%CC% %CFLAGS% lib/syslocale_os.cpp -o syslocale_os.o
%CC% %CFLAGS% lib/sysinfo.cpp -o sysinfo.o
%CC% %CFLAGS% lib/syslib.cpp -o syslib.o
%CC% %CFLAGS% lib/getopt.cpp -o getopt.o

%CC% %CFLAGS% run_dump.cpp -o run_dump.o
%CC% %CFLAGS% run_echo.cpp -o run_echo.o
%CC% %CFLAGS% run_cat.cpp -o run_cat.o
%CC% %CFLAGS% run_sysinfo.cpp -o run_sysinfo.o
%CC% %CFLAGS% run_find.cpp -o run_find.o
%CC% %CFLAGS% run_ask.cpp -o run_ask.o
%CC% %CFLAGS% run_ls.cpp -o run_ls.o

%CC% %CFLAGS% run_mkdir.cpp -o run_mkdir.o
%CC% %CFLAGS% run_rmdir.cpp -o run_rmdir.o
%CC% %CFLAGS% run_mv.cpp -o run_mv.o
%CC% %CFLAGS% run_rm.cpp -o run_rm.o

%CC% %CFLAGS% test_getopt.cpp -o test_getopt.o
%CC% %CFLAGS% test_echo.cpp -o test_echo.o
%CC% %CFLAGS% test_locale.cpp -o test_locale.o

rem link

%CC% %LDFLAGS% getopt.o iolib.o run_dump.o -o run-dump.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_echo.o -o run-echo.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat.exe
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_ask.o -o run-ask.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o run_ls.o -o run-ls
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mv.o -o run-mv.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rm.o -o run-rm.exe

%CC% %LDFLAGS% getopt.o test_getopt.o -o test-getopt
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_echo.o -o test-echo.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_locale.o -o test-locale
