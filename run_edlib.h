#include <stdlib.h>
#include <stdbool.h>

#define RUN_ED_BY_STRING 1
#define RUN_ED_BY_FILE   2

/* ED encode function */
typedef int (*lib_ed_encode_func) (int algo, const char* idata, size_t isize, char** odata, size_t* osize);

/* ED decode function */
typedef int (*lib_ed_decode_func) (int algo, const char* idata, size_t isize, char** odata, size_t* osize);

/* ED get algo function */
typedef int (*lib_ed_get_func) (char* name);

/* ED list algo function */
typedef void (*lib_ed_list_func) ();

/* Run ED configuration */
typedef struct lib_ed_config_t {
    const char* name;               /* ED name: base32            */
    lib_ed_encode_func encode_func; /* ED encode function         */
    lib_ed_decode_func decode_func; /* ED decode function         */
    lib_ed_get_func get_func;       /* ED get algorithm function  */
    lib_ed_list_func list_func;     /* ED list algorithm function */
    int mode;                       /* by string/file             */
    int def_algo;                   /* Algorithm by default       */
} lib_ed_config_t;

int run_ed(lib_ed_config_t* config, int argc, char* argv[]);

int _lib_stricmp(const char* str1, const char* str2);

