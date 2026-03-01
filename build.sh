CC=gcc
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
$CC $CFLAGS lib/memlib.c -o memlib.o
$CC $CFLAGS lib/strlib.c -o strlib.o
$CC $CFLAGS lib/wstrlib.c -o wstrlib.o
$CC $CFLAGS lib/pathlib.c -o pathlib.o
$CC $CFLAGS lib/wclib.c -o wclib.o

$CC $CFLAGS lib/fspath.c -o fspath.o
$CC $CFLAGS lib/fsdirent.c -o fsdirent.o
$CC $CFLAGS lib/fsfile.c -o fsfile.o
$CC $CFLAGS lib/fslib.c -o fslib.o

$CC $CFLAGS lib/iodef.c -o iodef.o
$CC $CFLAGS lib/iolib.c -o iolib.o

$CC $CFLAGS lib/dmplib.c -o dmplib.o
$CC $CFLAGS lib/fmtlib.c -o fmtlib.o
$CC $CFLAGS lib/socketlib.c -o socketlib.o
$CC $CFLAGS lib/nflib.c -o nflib.o
$CC $CFLAGS lib/asklib.c -o asklib.o

$CC $CFLAGS lib/syscpu.c -o syscpu.o
$CC $CFLAGS lib/sysos.c -o sysos.o
$CC $CFLAGS lib/sysuser.c -o sysuser.o
$CC $CFLAGS lib/syslocale.c -o syslocale.o
$CC $CFLAGS lib/syslocale_os.c -o syslocale_os.o
$CC $CFLAGS lib/sysinfo.c -o sysinfo.o
$CC $CFLAGS lib/syslib.c -o syslib.o

$CC $CFLAGS lib/getopt.c -o getopt.o
$CC $CFLAGS lib/utf8lib.c -o utf8lib.o
$CC $CFLAGS lib/unimap.c -o unimap.o
$CC $CFLAGS lib/encdef.c -o encdef.o
$CC $CFLAGS lib/enclib.c -o enclib.o
$CC $CFLAGS lib/base16.c -o base16.o
$CC $CFLAGS lib/base32.c -o base32.o
$CC $CFLAGS lib/base58.c -o base58.o
$CC $CFLAGS lib/base64.c -o base64.o

$CC $CFLAGS lib/uuid.c -o uuid.o

$CC $CFLAGS lib/common.c -o common.o
$CC $CFLAGS lib/debug.c -o debug.o

$CC $CFLAGS lib/array.c -o array.o
$CC $CFLAGS lib/list.c -o list.o
$CC $CFLAGS lib/set.c -o set.o
$CC $CFLAGS lib/stack.c -o stack.o
$CC $CFLAGS lib/map.c -o map.o

$CC $CFLAGS lib/md.c -o md.o
$CC $CFLAGS lib/md5.c -o md5.o
$CC $CFLAGS lib/sha1.c -o sha1.o
$CC $CFLAGS lib/sha256.c -o sha256.o
$CC $CFLAGS lib/sha512.c -o sha512.o

if [[ "$OSTYPE" == "darwin"* ]]; then
   $CC $CFLAGS lib/sysos_mac.m -o sysos_mac.o
fi

$CC $CFLAGS run_echo.c -o run_echo.o
$CC $CFLAGS run_dump.c -o run_dump.o
$CC $CFLAGS run_cat.c -o run_cat.o
$CC $CFLAGS run_sysinfo.c -o run_sysinfo.o
$CC $CFLAGS run_find.c -o run_find.o
$CC $CFLAGS run_ask.c -o run_ask.o
$CC $CFLAGS run_ls.c -o run_ls.o

$CC $CFLAGS run_mkdir.c -o run_mkdir.o
$CC $CFLAGS run_rmdir.c -o run_rmdir.o
$CC $CFLAGS run_mv.c -o run_mv.o
$CC $CFLAGS run_rm.c -o run_rm.o
$CC $CFLAGS run_cp.c -o run_cp.o

$CC $CFLAGS run_get.c -o run_get.o

$CC $CFLAGS run_mdlib.c -o run_mdlib.o

$CC $CFLAGS run_md5.c -o run_md5.o
$CC $CFLAGS run_sha1.c -o run_sha1.o
$CC $CFLAGS run_sha224.c -o run_sha224.o
$CC $CFLAGS run_sha256.c -o run_sha256.o
$CC $CFLAGS run_sha384.c -o run_sha384.o
$CC $CFLAGS run_sha512.c -o run_sha512.o
$CC $CFLAGS run_hash.c -o run_hash.o

$CC $CFLAGS run_hex2dat.c -o run_hex2dat.o
#$CC $CFLAGS run_ssldump.c -o run_ssldump.o

$CC $CFLAGS run_unimap.c -o run_unimap.o
$CC $CFLAGS run_iconv.c -o run_iconv.o

$CC $CFLAGS run_ctype.c -o run_ctype.o

$CC $CFLAGS run_basename.c -o run_basename.o
$CC $CFLAGS run_dirname.c -o run_dirname.o
$CC $CFLAGS run_realpath.c -o run_realpath.o
$CC $CFLAGS run_pwd.c -o run_pwd.o

$CC $CFLAGS run_edlib.c -o run_edlib.o

$CC $CFLAGS run_base16.c -o run_base16.o
$CC $CFLAGS run_base32.c -o run_base32.o
$CC $CFLAGS run_base58.c -o run_base58.o
$CC $CFLAGS run_base64.c -o run_base64.o
$CC $CFLAGS run_encode.c -o run_encode.o

$CC $CFLAGS run_ascii.c -o run_ascii.o
$CC $CFLAGS run_uuid.c -o run_uuid.o
$CC $CFLAGS run_diff.c -o run_diff.o
$CC $CFLAGS run_tok.c -o run_tok.o

$CC $CFLAGS run_hex2dec.c -o run_hex2dec.o
$CC $CFLAGS run_hex2oct.c -o run_hex2oct.o
$CC $CFLAGS run_hex2bin.c -o run_hex2bin.o

$CC $CFLAGS run_dec2hex.c -o run_dec2hex.o
$CC $CFLAGS run_dec2oct.c -o run_dec2oct.o
$CC $CFLAGS run_dec2bin.c -o run_dec2bin.o

$CC $CFLAGS run_oct2hex.c -o run_oct2hex.o
$CC $CFLAGS run_oct2dec.c -o run_oct2dec.o
$CC $CFLAGS run_oct2bin.c -o run_oct2bin.o

$CC $CFLAGS run_bin2hex.c -o run_bin2hex.o
$CC $CFLAGS run_bin2dec.c -o run_bin2dec.o
$CC $CFLAGS run_bin2oct.c -o run_bin2oct.o

$CC $CFLAGS run_dconv.c -o run_dconv.o

$CC $CFLAGS run_txt2html.c -o run_txt2html.o
$CC $CFLAGS run_txt2svg.c -o run_txt2svg.o
$CC $CFLAGS run_txt2rtf.c -o run_txt2rtf.o
$CC $CFLAGS run_txt2pdf.c -o run_txt2pdf.o

$CC $CFLAGS test_getopt.c -o test_getopt.o
$CC $CFLAGS test_echo.c -o test_echo.o
$CC $CFLAGS test_locale.c -o test_locale.o
$CC $CFLAGS test_utf8.c -o test_utf8.o
$CC $CFLAGS test_enc.c -o test_enc.o
$CC $CFLAGS test_strlib.c -o test_strlib.o
$CC $CFLAGS test_buf.c -o test_buf.o
$CC $CFLAGS test_scandir.c -o test_scandir.o
$CC $CFLAGS test_map.c -o test_map.o
$CC $CFLAGS test_data.c -o test_data.o

# link
###########
$CC $LDFLAGS getopt.o strlib.o syslocale.o syslocale_os.o syslib.o iolib.o run_echo.o -o run-echo
$CC $LDFLAGS getopt.o iodef.o iolib.o dmplib.o run_dump.o -o run-dump
$CC $LDFLAGS iodef.o iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o run_cat.o -o run-cat
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o syscpu.o sysos.o sysuser.o syslocale.o syslocale_os.o sysinfo.o syslib.o run_sysinfo.o -o run-sysinfo
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_find.o -o run-find
$CC $LDFLAGS $OBJS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o iolib.o asklib.o syslocale.o syslocale_os.o syslib.o run_ask.o -o run-ask
$CC $LDFLAGS getopt.o strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o iodef.o fmtlib.o syslocale.o syslocale_os.o syslib.o run_ls.o -o run-ls

$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o syslocale.o syslocale_os.o syslib.o run_mkdir.o -o run-mkdir
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o syslocale.o syslocale_os.o syslib.o run_rmdir.o -o run-rmdir
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o syslocale.o syslocale_os.o syslib.o run_mv.o -o run-mv
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o syslocale.o syslocale_os.o syslib.o run_rm.o -o run-rm
$CC $LDFLAGS strlib.o wstrlib.o pathlib.o wclib.o fspath.o fsdirent.o fsfile.o fslib.o syslocale.o syslocale_os.o syslib.o run_cp.o -o run-cp

$CC $LDFLAGS common.o debug.o iodef.o strlib.o socketlib.o nflib.o run_get.o -o run-get

$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o md5.o run_md5.o -o run-md5
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o sha1.o run_sha1.o -o run-sha1
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o sha256.o run_sha224.o -o run-sha224
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o sha256.o run_sha256.o -o run-sha256
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o sha512.o run_sha384.o -o run-sha384
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o sha512.o run_sha512.o -o run-sha512
$CC $LDFLAGS getopt.o common.o memlib.o iolib.o run_mdlib.o md5.o sha1.o sha256.o sha512.o run_hash.o -o run-hash

$CC $LDFLAGS getopt.o iodef.o iolib.o run_hex2dat.o -o run-hex2dat
#$CC $LDFLAGS getopt.o iodef.o iolib.o run_ssldump.o -o run-ssldump

$CC $LDFLAGS getopt.o iolib.o run_unimap.o -o run-unimap
$CC $LDFLAGS getopt.o iolib.o unimap.o utf8lib.o encdef.o enclib.o run_iconv.o -o run-iconv

$CC $LDFLAGS run_ctype.o -o run-ctype

$CC $LDFLAGS getopt.o pathlib.o run_basename.o -o run-basename
$CC $LDFLAGS getopt.o pathlib.o run_dirname.o -o run-dirname
$CC $LDFLAGS getopt.o fspath.o run_realpath.o -o run-realpath
$CC $LDFLAGS getopt.o fspath.o run_pwd.o -o run-pwd
$CC $LDFLAGS getopt.o iolib.o base16.o run_edlib.o run_base16.o -o run-base16
$CC $LDFLAGS getopt.o iolib.o base32.o run_edlib.o run_base32.o -o run-base32
$CC $LDFLAGS getopt.o iolib.o base58.o run_edlib.o run_base58.o -o run-base58
$CC $LDFLAGS getopt.o iolib.o base64.o run_edlib.o run_base64.o -o run-base64
$CC $LDFLAGS getopt.o iolib.o base16.o base32.o base58.o base64.o run_edlib.o run_encode.o -o run-encode

$CC $LDFLAGS getopt.o run_ascii.o -o run-ascii
$CC $LDFLAGS getopt.o memlib.o md5.o sha1.o uuid.o run_uuid.o -o run-uuid
$CC $LDFLAGS getopt.o run_diff.o -o run-diff
$CC $LDFLAGS getopt.o iolib.o run_tok.o -o run-tok

$CC $LDFLAGS getopt.o run_hex2dec.o -o run-hex2dec
$CC $LDFLAGS getopt.o run_hex2oct.o -o run-hex2oct
$CC $LDFLAGS getopt.o run_hex2bin.o -o run-hex2bin

$CC $LDFLAGS getopt.o run_dec2hex.o -o run-dec2hex
$CC $LDFLAGS getopt.o run_dec2oct.o -o run-dec2oct
$CC $LDFLAGS getopt.o run_dec2bin.o -o run-dec2bin

$CC $LDFLAGS getopt.o run_oct2hex.o -o run-oct2hex
$CC $LDFLAGS getopt.o run_oct2dec.o -o run-oct2dec
$CC $LDFLAGS getopt.o run_oct2bin.o -o run-oct2bin

$CC $LDFLAGS getopt.o run_bin2hex.o -o run-bin2hex
$CC $LDFLAGS getopt.o run_bin2dec.o -o run-bin2dec
$CC $LDFLAGS getopt.o run_bin2oct.o -o run-bin2oct

$CC $LDFLAGS getopt.o run_dconv.o -o run-dconv

$CC $LDFLAGS getopt.o iodef.o iolib.o run_txt2html.o -o run-txt2html
$CC $LDFLAGS getopt.o iodef.o iolib.o run_txt2svg.o -o run-txt2svg
$CC $LDFLAGS getopt.o iodef.o iolib.o run_txt2rtf.o -o run-txt2rtf
$CC $LDFLAGS getopt.o iodef.o iolib.o unimap.o utf8lib.o encdef.o enclib.o run_txt2pdf.o -o run-txt2pdf

$CC $LDFLAGS getopt.o test_getopt.o -o test-getopt
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_echo.o -o test-echo
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o test_locale.o -o test-locale
$CC $LDFLAGS iolib.o strlib.o wstrlib.o syslocale.o syslocale_os.o syslib.o utf8lib.o test_utf8.o -o test-utf8
$CC $LDFLAGS encdef.o test_enc.o -o test-enc
$CC $LDFLAGS strlib.o test_strlib.o -o test-strlib
$CC $LDFLAGS test_buf.o -o test-buf
$CC $LDFLAGS fsdirent.o test_scandir.o -o test-scandir
$CC $LDFLAGS test_map.o -o test-map
$CC $LDFLAGS memlib.o array.o list.o map.o test_data.o -o test-data
