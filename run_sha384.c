#include "sha512.h"
#include "run_mdlib.h"

#define MD_NAME LIB_MD_SHA384_NAME // "SHA384"
#define MD_SIZE LIB_MD_SHA384_SIZE // 48

int _lib_sha384(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 1);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;
    config.md_func = _lib_sha384;

    return run_md(&config, argc, argv);
}