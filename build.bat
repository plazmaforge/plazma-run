set "LIBS=-lversion"

rem clean
call clean.bat

rem compile

g++ -c strlib.cpp -o strlib.o
g++ -c wstrlib.cpp -o wstrlib.o
g++ -c pathlib.cpp -o pathlib.o
g++ -c fslib.cpp -o fslib.o
g++ -c wclib.cpp -o wclib.o
g++ -c iolib.cpp -o iolib.o
g++ -c srhlib.cpp -o srhlib.o
g++ -c syscpu.cpp -o syscpu.o
g++ -c sysos.cpp -o sysos.o
g++ -c sysuser.cpp -o sysuser.o
g++ -c syslocale.cpp -o syslocale.o
g++ -c syslocale_os.cpp -o syslocale_os.o
g++ -c sysinfo.cpp -o sysinfo.o
g++ -c syslib.cpp -o syslib.o
g++ -c getopt.cpp -o getopt.o

g++ -c run_dump.cpp -o run_dump.o
g++ -c run_echo.cpp -o run_echo.o
g++ -c run_cat.cpp -o run_cat.o
g++ -c run_sysinfo.cpp -o run_sysinfo.o
g++ -c run_find.cpp -o run_find.o
g++ -c run_ls.cpp -o run_ls.o

g++ -c run_mkdir.cpp -o run_mkdir.o
g++ -c run_rmdir.cpp -o run_rmdir.o

rem link

g++ iolib.o run_dump.o -o run-dump.exe
g++ iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_echo.o -o run-echo.exe
g++ iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat.exe
g++ getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo.exe %LIBS%
g++ getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o srhlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_find.o -o run-find.exe %LIBS%

g++ getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o run_ls.o -o run-ls

g++ strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir.exe
g++ strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir.exe