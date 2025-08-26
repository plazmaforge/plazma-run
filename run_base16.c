#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#include <string.h>

#include "getopt.h"
#include "base16.h"
#include "iolib.h"
#include "run_edlib.h"
 
static int _encode_algo(int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
    return lib_base16_encode(idata, isize, odata, osize);
}

static int _decode_algo(int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
    return lib_base16_decode(idata, isize, odata, osize);
}

int main(int argc, char* argv[]) {

    lib_ed_config_t config;
    config.name        = "base16";
    config.encode_func = _encode_algo;
    config.decode_func = _decode_algo;
    config.get_func    = NULL;
    config.list_func   = NULL;
    config.def_algo    = 0;

    return run_ed(&config, argc, argv);
}
