#include "sha512.h"
#include "run_mdlib.h"

int _lib_sha384(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 1);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = LIB_MD_SHA384_NAME; // "SHA384";
    config.md_size = LIB_MD_SHA384_SIZE; // 48;
    config.md_func = _lib_sha384;

    return run_md(&config, argc, argv);
}