#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fslib.h"

void usage() {
    printf("Usage: run-rmdir <dir>\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    char* dir_name = argv[1];
    int retval = fs_rmdir(dir_name);
    if (retval != 0) {
        fprintf(stderr, "Can't remove directory: %s\n", dir_name);
    }
    return 0;
}
