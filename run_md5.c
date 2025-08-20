#include "md5.h"
#include "run_mdlib.h"

int main(int argc, char* argv[]) {
 
    lib_md_config_t config;
    config.md_name = LIB_MD_MD5_NAME; // "MD5";
    config.md_size = LIB_MD_MD5_SIZE; // 16;
    config.md_func = lib_md5;
    config.md_init = NULL;

    return run_md(&config, argc, argv);

}