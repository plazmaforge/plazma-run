#include "sha1.h"
#include "run_mdlib.h"

#define MD_NAME LIB_MD_SHA1_NAME // "SHA1"
#define MD_SIZE LIB_MD_SHA1_SIZE // 20

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;
    config.md_func = lib_sha1;

    return run_md(&config, argc, argv);
}