#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// #include "getopt.h"
//#include "strlib.h"
//#include "wstrlib.h"
//#include "syslib.h"
//#include "sysinfo.h"

#include "fsdirent.h"

void usage() {
    printf("Usage: test-scandir <dirname>\n");
}

int test_scandir(const char* dirname) {
    if (!dirname) {
        dirname = ".";
    }

    printf("dirname: %s\n", dirname);

    struct dirent **list;
    int size = 0;

    size = lib_scandir(dirname, &list, NULL, lib_dalphasort);

    if (size < 0) {
        fprintf(stderr, "Can't scan directory: %s\n", dirname);
        return 1;
    }

    printf("total  : %d\n", size);
    for (int i = 0; i < size; i++) {
        struct dirent* ent = list[i];
        printf("%d: %s %s\n", (i + 1), (ent->d_type == LIB_FS_DIR ? "[D]" : "   "), ent->d_name);

    }

    lib_dirents_free(list, size);


    return 0;


}

int main(int argc, char* argv[]) {
    const char* dirname = NULL;
    if (argc < 2) {
        dirname = argv[1]; 
    }

    //lib_sys_locale_init();

    int retval = test_scandir(dirname);

    //lib_sys_locale_restore();

    return retval < 0 ? 1 : 0;
}