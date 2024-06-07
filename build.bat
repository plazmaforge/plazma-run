set "CC=g++"
set "CFLAGS=-c -Ilib"
set "LDFLAGS="
set "LIBS=-lversion -lws2_32"

rem clean
call clean.bat

rem compile

%CC% %CFLAGS% lib/strlib.cpp -o strlib.o
%CC% %CFLAGS% lib/wstrlib.cpp -o wstrlib.o
%CC% %CFLAGS% lib/pathlib.cpp -o pathlib.o
%CC% %CFLAGS% lib/fslib.cpp -o fslib.o
%CC% %CFLAGS% lib/wclib.cpp -o wclib.o
%CC% %CFLAGS% lib/iolib.cpp -o iolib.o
%CC% %CFLAGS% lib/dumplib.cpp -o dumplib.o
%CC% %CFLAGS% lib/fmtlib.cpp -o fmtlib.o
%CC% %CFLAGS% lib/socketlib.cpp -o socketlib.o
%CC% %CFLAGS% lib/netlib.cpp -o netlib.o
%CC% %CFLAGS% lib/asklib.cpp -o asklib.o
%CC% %CFLAGS% lib/syscpu.cpp -o syscpu.o
%CC% %CFLAGS% lib/sysos.cpp -o sysos.o
%CC% %CFLAGS% lib/sysuser.cpp -o sysuser.o
%CC% %CFLAGS% lib/syslocale.cpp -o syslocale.o
%CC% %CFLAGS% lib/syslocale_os.cpp -o syslocale_os.o
%CC% %CFLAGS% lib/sysinfo.cpp -o sysinfo.o
%CC% %CFLAGS% lib/syslib.cpp -o syslib.o
%CC% %CFLAGS% lib/getopt.cpp -o getopt.o

%CC% %CFLAGS% lib/common.cpp -o common.o
%CC% %CFLAGS% lib/debug.cpp -o debug.o

%CC% %CFLAGS% lib/md.cpp -o md.o
%CC% %CFLAGS% lib/md5.cpp -o md5.o
%CC% %CFLAGS% lib/sha256.cpp -o sha256.o
%CC% %CFLAGS% lib/sha512.cpp -o sha512.o

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
%CC% %CFLAGS% run_cp.cpp -o run_cp.o

%CC% %CFLAGS% run_get.cpp -o run_get.o

%CC% %CFLAGS% run_md5.cpp -o run_md5.o
%CC% %CFLAGS% run_sha224.cpp -o run_sha224.o
%CC% %CFLAGS% run_sha256.cpp -o run_sha256.o
%CC% %CFLAGS% run_sha384.cpp -o run_sha384.o
%CC% %CFLAGS% run_sha512.cpp -o run_sha512.o

%CC% %CFLAGS% run_hex_byte.cpp -o run_hex_byte.o
%CC% %CFLAGS% run_ssl_dump.cpp -o run_ssl_dump.o

%CC% %CFLAGS% test_getopt.cpp -o test_getopt.o
%CC% %CFLAGS% test_echo.cpp -o test_echo.o
%CC% %CFLAGS% test_locale.cpp -o test_locale.o

rem link

%CC% %LDFLAGS% getopt.o iolib.o dumplib.o run_dump.o -o run-dump.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_echo.o -o run-echo.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat.exe
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_ask.o -o run-ask.exe %LIBS%
%CC% %LDFLAGS% getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o fmtlib.o run_ls.o -o run-ls
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mv.o -o run-mv.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rm.o -o run-rm.exe
%CC% %LDFLAGS% strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_cp.o -o run-cp.exe

%CC% %LDFLAGS% common.o debug.o strlib.o socketlib.o netlib.o run_get.o -o run-get.exe %LIBS%

%CC% %LDFLAGS% getopt.o common.o iolib.o md5.o run_md5.o -o run-md5.exe
%CC% %LDFLAGS% common.o sha256.o run_sha224.o -o run-sha224.exe
%CC% %LDFLAGS% common.o sha256.o run_sha256.o -o run-sha256.exe
%CC% %LDFLAGS% common.o sha512.o run_sha384.o -o run-sha384.exe
%CC% %LDFLAGS% common.o sha512.o run_sha512.o -o run-sha512.exe

%CC% %LDFLAGS% getopt.o iolib.o run_hex_byte.o -o run-hex-byte.exe
%CC% %LDFLAGS% getopt.o iolib.o run_ssl_dump.o -o run-ssl-dump.exe

%CC% %LDFLAGS% getopt.o test_getopt.o -o test-getopt.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_echo.o -o test-echo.exe
%CC% %LDFLAGS% iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_locale.o -o test-locale
