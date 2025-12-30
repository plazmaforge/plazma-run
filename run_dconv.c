#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "run_dclib.h"

void usage() {
    fprintf(stderr, "Usage: run-dconv -f fromtype -t totype value\n");
}

static void print_list() {
    fprintf(stdout, " 2, bin\n");
    fprintf(stdout, " 8, oct\n");
    fprintf(stdout, "10, dec\n");
    fprintf(stdout, "16, hex\n");
}

int main(int argc, char* argv[]) {

    lib_cli_prog_init(argv);

    int argm = 1;
    if (lib_cli_not_argind(argc, argm)) {        
        usage();
        return 1;
    }
    
    int error = 0;
    int opt;

    char* from_type = NULL;
    char* to_type   = NULL;
    int from_base   = 0;
    int to_base     = 0;

    bool opt_list   = false;

    while ((opt = lib_getopt(argc, argv, "lf:t:")) != -1) {
        switch (opt) {
        case 'l':
            opt_list = true;
            break;
        case 'f':
            from_type = optarg;
            break;
        case 't':
            to_type = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }

        if (opt_list) {
            break;
        }
    }

    if (opt_list) {
        print_list();
        return 0;
    }

    if (!from_type || !to_type) {
        error = 1;
    }

    if (error) {
        usage();
        return 1;
    }

    return 0;
}
