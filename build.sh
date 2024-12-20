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
$CC $CFLAGS lib/pathlib.cpp -o pathlib.o
$CC $CFLAGS lib/wclib.cpp -o wclib.o
$CC $CFLAGS lib/fslib.cpp -o fslib.o
$CC $CFLAGS lib/iodef.cpp -o iodef.o
$CC $CFLAGS lib/iolib.cpp -o iolib.o
$CC $CFLAGS lib/dmplib.cpp -o dmplib.o
$CC $CFLAGS lib/fmtlib.cpp -o fmtlib.o
$CC $CFLAGS lib/socketlib.cpp -o socketlib.o
$CC $CFLAGS lib/nflib.cpp -o nflib.o
$CC $CFLAGS lib/asklib.cpp -o asklib.o
$CC $CFLAGS lib/syscpu.cpp -o syscpu.o
$CC $CFLAGS lib/sysos.cpp -o sysos.o
$CC $CFLAGS lib/sysuser.cpp -o sysuser.o
$CC $CFLAGS lib/syslocale.cpp -o syslocale.o
$CC $CFLAGS lib/syslocale_os.cpp -o syslocale_os.o
$CC $CFLAGS lib/sysinfo.cpp -o sysinfo.o
$CC $CFLAGS lib/syslib.cpp -o syslib.o
$CC $CFLAGS lib/getopt.cpp -o getopt.o
$CC $CFLAGS lib/utf8lib.cpp -o utf8lib.o
$CC $CFLAGS lib/unimap.cpp -o unimap.o
$CC $CFLAGS lib/encdef.cpp -o encdef.o
$CC $CFLAGS lib/enclib.cpp -o enclib.o

$CC $CFLAGS lib/common.cpp -o common.o
$CC $CFLAGS lib/debug.cpp -o debug.o

$CC $CFLAGS lib/md.cpp -o md.o
$CC $CFLAGS lib/md5.cpp -o md5.o
$CC $CFLAGS lib/sha256.cpp -o sha256.o
$CC $CFLAGS lib/sha512.cpp -o sha512.o

if [[ "$OSTYPE" == "darwin"* ]]; then
   $CC $CFLAGS lib/sysos_mac.mm -o sysos_mac.o
fi

$CC $CFLAGS run_dump.cpp -o run_dump.o
$CC $CFLAGS run_echo.cpp -o run_echo.o
$CC $CFLAGS run_cat.cpp -o run_cat.o
$CC $CFLAGS run_sysinfo.cpp -o run_sysinfo.o
$CC $CFLAGS run_find.cpp -o run_find.o
$CC $CFLAGS run_ask.cpp -o run_ask.o
$CC $CFLAGS run_ls.cpp -o run_ls.o

$CC $CFLAGS run_mkdir.cpp -o run_mkdir.o
$CC $CFLAGS run_rmdir.cpp -o run_rmdir.o
$CC $CFLAGS run_mv.cpp -o run_mv.o
$CC $CFLAGS run_rm.cpp -o run_rm.o
$CC $CFLAGS run_cp.cpp -o run_cp.o

$CC $CFLAGS run_get.cpp -o run_get.o

$CC $CFLAGS run_mdlib.cpp -o run_mdlib.o

$CC $CFLAGS run_md5.cpp -o run_md5.o
$CC $CFLAGS run_sha224.cpp -o run_sha224.o
$CC $CFLAGS run_sha256.cpp -o run_sha256.o
$CC $CFLAGS run_sha384.cpp -o run_sha384.o
$CC $CFLAGS run_sha512.cpp -o run_sha512.o

$CC $CFLAGS run_hex_byte.cpp -o run_hex_byte.o
$CC $CFLAGS run_ssldump.cpp -o run_ssldump.o

$CC $CFLAGS run_unimap.cpp -o run_unimap.o
$CC $CFLAGS run_iconv.cpp -o run_iconv.o

$CC $CFLAGS run_ctype.cpp -o run_ctype.o

$CC $CFLAGS test_getopt.cpp -o test_getopt.o
$CC $CFLAGS test_echo.cpp -o test_echo.o
$CC $CFLAGS test_locale.cpp -o test_locale.o
$CC $CFLAGS test_utf8.cpp -o test_utf8.o
$CC $CFLAGS test_enc.cpp -o test_enc.o
$CC $CFLAGS test_strlib.cpp -o test_strlib.o

# link
###########
$CC $LDFLAGS getopt.o iodef.o iolib.o dmplib.o run_dump.o -o run-dump
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_echo.o -o run-echo
$CC $LDFLAGS iodef.o iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_ask.o -o run-ask
$CC $LDFLAGS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fslib.o iodef.o fmtlib.o syslocale.o syslocale_os.o syslib.o run_ls.o -o run-ls
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_mv.o -o run-mv
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_rm.o -o run-rm
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fslib.o syslocale.o syslocale_os.o syslib.o run_cp.o -o run-cp

$CC $LDFLAGS common.o debug.o iodef.o strlib.o socketlib.o nflib.o run_get.o -o run-get

$CC $LDFLAGS getopt.o common.o iolib.o run_mdlib.o md5.o run_md5.o -o run-md5
$CC $LDFLAGS getopt.o common.o iolib.o run_mdlib.o sha256.o run_sha224.o -o run-sha224
$CC $LDFLAGS getopt.o common.o iolib.o run_mdlib.o sha256.o run_sha256.o -o run-sha256
$CC $LDFLAGS getopt.o common.o iolib.o run_mdlib.o sha512.o run_sha384.o -o run-sha384
$CC $LDFLAGS getopt.o common.o iolib.o run_mdlib.o sha512.o run_sha512.o -o run-sha512

$CC $LDFLAGS getopt.o iodef.o iolib.o run_hex_byte.o -o run-hex-byte
$CC $LDFLAGS getopt.o iodef.o iolib.o run_ssldump.o -o run-ssldump

$CC $LDFLAGS getopt.o iolib.o run_unimap.o -o run-unimap
$CC $LDFLAGS getopt.o iolib.o unimap.o utf8lib.o encdef.o enclib.o run_iconv.o -o run-iconv

$CC $LDFLAGS run_ctype.o -o run-ctype

$CC $LDFLAGS getopt.o test_getopt.o -o test-getopt
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_echo.o -o test-echo
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_locale.o -o test-locale
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o utf8lib.o test_utf8.o -o test-utf8
$CC $LDFLAGS encdef.o test_enc.o -o test-enc
$CC $LDFLAGS strlib.o test_strlib.o -o test-strlib
