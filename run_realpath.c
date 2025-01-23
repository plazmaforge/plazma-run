#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"
#include "wclib.h"
#include "fslib.h"

static bool _lib_stremp(char* str) {
    return (!str || *str == '\0');
}

int run_realpath(char* path) {
    if (_lib_stremp(path)) {
        return 1;
    }
    char* name = lib_fs_get_real_path(path);
    if (_lib_stremp(name)) {
        return 1;
    }
    fprintf(stdout, "%s\n", name);
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-realpath string\n");
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    char* path = NULL;  

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
    
    //fprintf(stderr, ">> path  : %s\n", path);

    int retval = run_realpath(path);
 
    //lib_sys_locale_restore();
    
    return retval;
}