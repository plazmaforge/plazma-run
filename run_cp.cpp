#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fslib.h"

void usage() {
    fprintf(stderr, "Usage: run-cp source_file target_file\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        usage();
        return 0;
    }
    char* src_file_name = argv[1];
    char* dst_file_name = argv[2];

    int retval = lib_fs_copy(src_file_name, dst_file_name);
    if (retval != 0) {
        fprintf(stderr, "Can't copy file from %s to %s: %s\n", src_file_name, dst_file_name, strerror(errno));
    }
    return 0;
}
