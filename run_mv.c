#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "clilib.h"
#include "fslib.h"

void usage() {
    fprintf(stderr, "Usage: run-mv source_file target_file\n");
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);

    char* src_file_name = argv[1];
    char* dst_file_name = argv[2];

    int retval = lib_fs_move(src_file_name, dst_file_name);
    if (retval != 0) {
        fprintf(stderr, "%s: Can't move file from %s to %s\n", prog_name, src_file_name, dst_file_name);
    }
    return 0;
}
