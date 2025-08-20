#include "sha512.h"
#include "run_mdlib.h"

int _lib_sha512(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 0);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = LIB_MD_SHA512_NAME; // "SHA512"
    config.md_size = LIB_MD_SHA512_SIZE; // 64;
    config.md_func = _lib_sha512;
    config.md_init = NULL;

    return run_md(&config, argc, argv);
}