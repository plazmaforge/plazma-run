#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fslib.h"

void usage() {
    printf("Usage: run-rm <file>\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    char* file_name = argv[1];
    int retval = lib_fs_remove(file_name);
    if (retval != 0) {
        fprintf(stderr, "Can't remove file: %s\n", file_name);
    }
    return 0;
}
