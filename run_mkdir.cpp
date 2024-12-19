#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arglib.h"
#include "fslib.h"

void usage() {
    fprintf(stderr, "Usage: run-mkdir directory\n");
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);

    char* dir_name = argv[1];
    int retval = lib_fs_create_dir(dir_name);
    if (retval != 0) {
        fprintf(stderr, "Can't create directory: %s\n", dir_name);
    }
    return 0;
}
