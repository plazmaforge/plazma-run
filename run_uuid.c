#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "getopt.h"
#include "clilib.h"
#include "uuid.h"

void run_uuid(int count, bool trailing) {

    lib_uuid_t u;
    for (int i = 0; i < count; i++) {
        lib_uuid_t u;
        lib_uuid_create(&u);
        lib_uuid_print(u);
        lib_uuid_reset();

        if (count > 1 && i < count - 1) {
            printf("\n");
        }
    }

    if (trailing) {
        printf("\n");
    }
}

void usage() {
    fprintf(stderr, "Usage: run-uuid [-n] [-c count]\n");
}

int main(int argc, char* argv[]) {
    
    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;

    int count = 1;
    bool trailing = true; // '\n'
    while ((opt = lib_getopt(argc, argv, "nc:")) != -1) {
        switch (opt) {
        case 'n':
            trailing = false;
            break;
        case 'c':
            count = atoi(optarg);
            if (count == 0) {
                fprintf(stderr, "%s: Incorrect count value: %s\n", prog_name, optarg);
                error = 1;
            }
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

    // if (argc - optind < min_arg) {
    //     fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
    //     usage();
    //     return 0;
    // }

    run_uuid(count, trailing);
    
    return 0;
}