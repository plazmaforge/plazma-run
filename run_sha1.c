#include "sha1.h"
#include "run_mdlib.h"

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = LIB_MD_SHA1_NAME; // "SHA1"
    config.md_size = LIB_MD_SHA1_SIZE; // 20;
    config.md_func = lib_sha1;
    config.md_init = NULL;

    return run_md(&config, argc, argv);
}