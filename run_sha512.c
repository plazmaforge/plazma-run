#include "sha512.h"
#include "run_mdlib.h"

#define MD_NAME LIB_MD_SHA512_NAME // "SHA512"
#define MD_SIZE LIB_MD_SHA512_SIZE // 64

int _lib_sha512(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 0);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;
    config.md_func = _lib_sha512;

    return run_md(&config, argc, argv);
}