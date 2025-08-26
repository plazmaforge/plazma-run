#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "getopt.h"

#include "base16.h"
#include "base32.h"
#include "base58.h"
#include "base64.h"

#include "iolib.h"
#include "run_edlib.h"

static int _encode_algo(int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
    if (algo == LIB_BASE16) {
        return lib_base16_encode(idata, isize, odata, osize);
    }
    if (algo >= LIB_BASE32_MIN && algo <= LIB_BASE32_MAX) {
        return lib_base32_encode_type(algo, idata, isize, odata, osize);
    }
    if (algo == LIB_BASE58) {
        return lib_base58_encode(idata, isize, odata, osize);
    }
    if (algo == LIB_BASE64) {
        return lib_base64_encode(idata, isize, odata, osize);
    }
    return -1;
}

static int _decode_algo(int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
    if (algo == LIB_BASE16) {
        return lib_base16_decode(idata, isize, odata, osize);
    }
    if (algo >= LIB_BASE32_MIN && algo <= LIB_BASE32_MAX) {
        return lib_base32_decode_type(algo, idata, isize, odata, osize);
    }
    if (algo == LIB_BASE58) {
        return lib_base58_decode(idata, isize, odata, osize);
    }
    if (algo == LIB_BASE64) {
        return lib_base64_decode(idata, isize, odata, osize);
    }
    return -1;
}

static int _get_algo(char* name) {
    if (!name)  {
        return -1;
    }

    if (_lib_stricmp(name, "base16") == 0) {
        return LIB_BASE16;
    } else if (_lib_stricmp(name, "base32") == 0) {
        return LIB_BASE32;
    } else if (_lib_stricmp(name, "base32u") == 0) {
        return LIB_BASE32U;
    } else if (_lib_stricmp(name, "base32l") == 0) {
        return LIB_BASE32L;
    } else if (_lib_stricmp(name, "zbase32") == 0) {
        return LIB_ZBASE32;
    } else if (_lib_stricmp(name, "base32hex") == 0) {
        return LIB_BASE32HEX;
    } else if (_lib_stricmp(name, "cbase32") == 0) {
        return LIB_CBASE32;
    } else if (_lib_stricmp(name, "base58") == 0) {
        return LIB_BASE58;
    } else if (_lib_stricmp(name, "base64") == 0) {
        return LIB_BASE64;
    }

    return -1;
}

static void _list_algo() {
    fprintf(stderr, "base16\n");
    fprintf(stderr, "base32\n");
    fprintf(stderr, "base32u\n");
    fprintf(stderr, "base32l\n");
    fprintf(stderr, "zbase32\n");
    fprintf(stderr, "base32hex\n");
    fprintf(stderr, "cbase32\n");
    fprintf(stderr, "base58\n");
    fprintf(stderr, "base64\n");
}

int main(int argc, char* argv[]) {

    lib_ed_config_t config;
    config.name        = "encode";
    config.encode_func = _encode_algo;
    config.decode_func = _decode_algo;
    config.get_func    = _get_algo;
    config.list_func   = _list_algo;
    config.def_algo    = 0;

    return run_ed(&config, argc, argv);
}
 