#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fslib.h"

void printUsage() {
    printf("Usage: run-mkdir <dir>\n");
    //printf("project-1   : %s\n", fs_get_dir_name("Users/user/Projects/project"));
    //printf("project-2   : %s\n", fs_get_dir_name("Users/user/Projects/project/"));
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }
    char* dir_name = argv[1];
    int retval = fs_create_dir(dir_name);
    if (retval != 0) {
        fprintf(stderr, "Can't create directory: %s\n", dir_name);
    }
    return 0;
}
