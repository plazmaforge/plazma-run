#include <stdlib.h>
#include <string.h>

const int RUN_MD_BY_STRING = 1;
const int RUN_MD_BY_FILE   = 2;

/* md function */
typedef int (*lib_md_func) (const unsigned char *input, size_t ilen, unsigned char output[]);

/* run md configuration */
typedef struct lib_md_config_t {
    const char* md_name; /* MD name: MD5                                                   */
    int md_size;         /* MD size                                                        */
    lib_md_func md_func; /* md function                                                    */
    int mode;            /* by string/file                                                 */
    bool is_title;       /* print with title: MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661 */
    bool is_upper;       /* upper case                                                     */
}
lib_md_config_t;

int run_md(lib_md_config_t* config, int argc, char* argv[]);

