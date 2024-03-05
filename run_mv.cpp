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

    // fs_file_t* dst_file = fs_get_file(dst_file_name);
    // bool is_dst_dir = false;
    // if (dst_file) {
    //     if (fs_file_is_dir(dst_file)) {
    //         is_dst_dir = true;
    //         char* dst_base_name = fs_get_base_name(src_file_name);          // [allocate]
    //         dst_file_name = fs_get_file_path(dst_file_name, dst_base_name); // [allocate]
    //         free(dst_base_name);
    //     }
    //     fs_file_free(dst_file);        
    // }
    // int retval = fs_rename(src_file_name, dst_file_name);
    // if (is_dst_dir) {
    //     free(dst_file_name);
    // }

    int retval = fs_move(src_file_name, dst_file_name);
    if (retval != 0) {
        fprintf(stderr, "Error [%i]. Can't move file form %s to %s\n", errno, src_file_name, dst_file_name);
    }
    return 0;
}
