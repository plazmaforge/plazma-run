#include <stdlib.h>
#include <string.h>

const int RUN_MD_BY_STRING = 1;
const int RUN_MD_BY_FILE   = 2;

typedef int (*lib_md_func) (const unsigned char *input, size_t ilen, unsigned char output[]);

typedef struct lib_md_config_t {
    const char* md_name;
    int md_size;
    lib_md_func md_func;
    int mode;
    bool is_title;
    bool is_upper;
}
lib_md_config_t;

static char* app_name;

int run_md(lib_md_config_t* config, int argc, char* argv[]);

