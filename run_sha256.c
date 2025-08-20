#include "sha256.h"
#include "run_mdlib.h"

int _lib_sha256(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha256(idata, isize, odata, 0);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = LIB_MD_SHA256_NAME; // "SHA256";
    config.md_size = LIB_MD_SHA256_SIZE; // 32;
    config.md_func = _lib_sha256;
    config.md_init = NULL;

    return run_md(&config, argc, argv);

}