
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "strlib.h"
#include "wstrlib.h"
#include "pathlib.h"
#include "wclib.h"
#include "fslib.h"
#include "syslib.h"

void usage() {
    fprintf(stderr, "Usage: run-find path\n");
    fprintf(stderr, "       run-find pattern\n");
}

void print_file_path(const char* path) {
    printf("%s\n", path);
}

void print_file(lib_fs_file_t* file) {
    if (!file) {
        return;
    }
    char* path = file->name ? lib_fs_get_real_path(file->name) : NULL;
    print_file_path(path);
    free(path);
}

void find_by_pattern(const char* dir_name, const char* pattern) {

    lib_fs_file_t** files = NULL;
    lib_fs_file_t* file = NULL;
    int file_count = lib_fs_scandir(dir_name, pattern, &files, LIB_FS_SCANDIR_RECURSIVE, true);

    printf("file_count: %d\n", file_count);

    if (file_count > 0) {
        for (int i = 0; i < file_count; i++) {
            file = files[i];
            if (!file) {
                // error
                continue;
            }
            print_file(file);
            //free(file);
            //fs_file_free(file);
        }
    }

    lib_fs_files_free(files);
}

void find_by_arg(const char* arg) {

    char* file_name = lib_strdup_uq(arg);
    char* dir_name = NULL;
    char* pattern = NULL;

    bool doit = false;

    int wildcard_index = lib_wc_get_wildcard_index(file_name);
    int path_index = 0;

    //printf(">> file_name: %s\n", file_name);
    //printf(">> wildcard_index: %d\n", wildcard_index);

    if (wildcard_index >= 0) {
        
        //printf(">> mode: wildcard\n");

        // dir1/dir2/file*
        // dir1/dir2/file?

        // file*
        // file?

        path_index = lib_wc_get_wildcard_path_index(wildcard_index, file_name);
        if (path_index >= 0) {

            // dir1/
            dir_name = lib_strndup(file_name, path_index + 1);

            // dir2/file*
            // dir2/file?

            file_name = file_name + path_index + 1;
        } else {

            // .   - Nix
            // ./* - Win
            dir_name = lib_strdup(lib_fs_get_current_find_path());
        }

        pattern = file_name;
        doit = true;

    } else {

        //printf(">> mode: path\n");

        char* path = lib_fs_get_real_path(file_name);
        if (path) {
            print_file_path(path);
            free(path);
        } else {
            dir_name = lib_fs_get_dir_name(file_name);
            if (!dir_name || strcmp(dir_name, ".") == 0) {

                // Scandir in current directory for one file
                if (dir_name) {
                    free(dir_name);
                }
                dir_name = lib_strdup(lib_fs_get_current_find_path());
                pattern = file_name;
                doit = true;

            } else {
                free(dir_name);
                dir_name = NULL;
            }
        }

    }

    //printf(">> dir    : %s\n", dir_name);
    //printf(">> file   : %s\n", file_name);
    //printf(">> pattern: %s\n", pattern);

    if (doit) {
        //printf(">> doit\n");
        find_by_pattern(dir_name, pattern);
    }

    if (file_name && path_index > 0) {
        file_name = file_name - path_index - 1;
    }

    free(dir_name);
    free(file_name);

}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    /*
    char* type = NULL;
    char* size = NULL;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "t:s:")) != -1) {
        switch (opt) {
        case 't':
            type = optarg;
            break;
        case 's':
            size = optarg;
            break;
        case '?':
            error = true;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind != min_arg) {
        printf("%s: Incorrect argument count\n", argv[0]);
        usage();
        return 0;
    }
                                        
    char* input = lib_strdup_uq(argv[optind]);
    char* file_name = lib_strdup_uq(argv[++optind]);

    //printf("input: %s\n", input);
    //printf("file : %s\n", file_name);

    size_t input_size = strlen(input);
    if (input_size == 0) {
        //printf("Input is empty\n");
        return 0;
    }
    */

    lib_locale_init();

    //printf(">> argc: %d\n",argc);
    for (int i = 1; i < argc; i++) {
        //printf(">> arg[%i]: %s\n", i, argv[i]);
        find_by_arg(argv[i]);
    }

    lib_locale_restore();
    return 0;

}
