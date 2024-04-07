#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fslib.h"

void usage() {
    printf("Usage: run-mv <src> <dst>\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        usage();
        return 0;
    }
    char* src_file_name = argv[1];
    char* dst_file_name = argv[2];

    int retval = lib_fs_move(src_file_name, dst_file_name);
    if (retval != 0) {
        fprintf(stderr, "Can't move file from %s to %s\n", src_file_name, dst_file_name);
    }
    return 0;
}
