#include "md5.h"
#include "run_mdlib.h"

#define MD_NAME "MD5"
#define MD_SIZE 16

int main(int argc, char* argv[]) {
 
    lib_md_config_t config;
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;
    config.md_func = lib_md5;

    return run_md(&config, argc, argv);

}