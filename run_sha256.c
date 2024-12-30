#include "sha256.h"
#include "run_mdlib.h"

#define MD_NAME "SHA256"
#define MD_SIZE 32

int _lib_sha256(const unsigned char *input, size_t ilen, unsigned char *output) {
    return lib_sha256(input, ilen, output, 0);
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;
    config.md_func = _lib_sha256;

    return run_md(&config, argc, argv);

}