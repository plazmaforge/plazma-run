#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "getopt.h"
#include "arglib.h"
#include "syslib.h"

void run_echo(int argc, char* argv[], int start, bool trailing) {
    for (int i = start; i < argc; i++) {
        if (i > start) {
            printf(" ");
        }
        printf("%s", argv[i]);
    }
    if (trailing) {
        printf("\n");
    }
}

void usage() {
    fprintf(stderr, "Usage: run-echo [-n] string ...\n");
}

int main(int argc, char* argv[]) {
    
    int min_arg = 1; // <string>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    bool trailing = true; // '\n'
    while ((opt = lib_getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':
            trailing = false;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }

    lib_sys_locale_init();

    run_echo(argc, argv, optind, trailing);

    lib_sys_locale_restore();
    
    return 0;
}