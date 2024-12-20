#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arglib.h"
#include "fslib.h"

void usage() {
    fprintf(stderr, "Usage: run-rm file\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);

    char* file_name = argv[1];
    int retval = lib_fs_remove(file_name);
    if (retval != 0) {
        fprintf(stderr, "%s: %s: Can't remove file\n", prog_name, file_name);
    }
    return 0;
}
