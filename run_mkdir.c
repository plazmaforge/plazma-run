#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clilib.h"
#include "fslib.h"

void usage() {
    fprintf(stderr, "Usage: run-mkdir directory\n");
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);

    char* dir_name = argv[1];
    int retval = lib_fs_create_dir(dir_name);
    if (retval != 0) {
        fprintf(stderr, "%s: %s: Can't create directory\n", prog_name, dir_name);
    }
    return 0;
}
