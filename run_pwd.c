#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "fslib.h"

static bool _lib_stremp(char* str) {
    return (!str || *str == '\0');
}

int run_pwd() {
    char* name = lib_fs_get_current_dir();
    if (_lib_stremp(name)) {
        return 1;
    }
    fprintf(stdout, "%s\n", name);
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-pwd\n");
}

int main(int argc, char* argv[]) {
    
    if (argc > 1) {
        usage();
        return 0;
    }

    int error = 0;
    int opt;

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

    //lib_sys_locale_init();

    int retval = run_pwd();
 
    //lib_sys_locale_restore();
    
    return retval;
}