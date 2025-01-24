#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "pathlib.h"

static bool _lib_stremp(char* str) {
    return (!str || *str == '\0');
}

static char* _lib_strrdel(char* str, char* sub) {
    if (!str) {
        return NULL;
    }
    if (!sub) {
        return str;
    }
    size_t total = strlen(str);
    if (total == 0) {
        return str;
    }
    size_t len = strlen(sub);
    if (len == 0 || len > total) {
        return str;
    }
    size_t offset = total - len;
    for (size_t i = 0; i < len; i++) {
        if (str[i + offset] != sub[i]) {
            return str;
        }
    }
    str[offset] = '\0'; // truncate string
    return str;
}

int run_basename(char* path, char* suffix) {
    if (_lib_stremp(path)) {
        return 1;
    }
    char* name = lib_path_base(path);
    if (_lib_stremp(name)) {
        return 1;
    }
    name = _lib_strrdel(name, suffix);
    if (_lib_stremp(name)) {
        return 1;
    }
    fprintf(stdout, "%s\n", name);
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-basename string [suffix]\n");
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;

    char* path = NULL;  
    char* suffix = NULL;

    /*
    while ((opt = lib_getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            suffix = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }
    */

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind < 1) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }

    //lib_sys_locale_init();

    path = argv[optind];
    
    if (++optind < argc) {
        suffix = argv[optind];
    }

    //fprintf(stderr, ">> path  : %s\n", path);
    //fprintf(stderr, ">> suffix: %s\n", suffix);

    int retval = run_basename(path, suffix);
 
    //lib_sys_locale_restore();
    
    return retval;
}