
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
    printf("Usage: run-find <text> <file> \n");
}

void print_file_path(const char* path) {
    printf("%s\n", path);
}

void print_file(fs_file_t* file) {
    if (!file) {
        return;
    }
    char* path = file->name ? lib_fs_get_real_path(file->name) : NULL;
    print_file_path(path);
    free(path);
}

void run_find(const char* dir_name, const char* pattern) {

    fs_file_t** files = NULL;
    fs_file_t* file = NULL;
    int file_count = lib_fs_scandir(dir_name, pattern, &files, FS_SCANDIR_RECURSIVE, true);

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

    lib_fs_files_free(files);
}

int main(int argc, char* argv[]) {

    /*
    if (argc < 2) {
        printf("%s: Incorrect argument count\n", argv[0]);
        usage();
        return 0;
    }

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

    char* file_name = lib_strdup_uq(argv[1]);
    char* dir_name = NULL;
    char* pattern = NULL;

    bool doit = false;

    init_locale();

    int wildcard_index = lib_wc_get_wildcard_index(file_name);

    if (wildcard_index >= 0) {

        int path_index = lib_wc_get_wildcard_path_index(wildcard_index, file_name);
        if (path_index >= 0) {
            dir_name = lib_strndup(file_name, path_index + 1);
            file_name = file_name + path_index + 1;
        } else {
            dir_name = lib_strdup(lib_fs_get_current_find_path());
        }

        pattern = file_name;
        doit = true;

    } else {

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
            }
        }

    }

    //printf("dir    : %s\n", dir_name);
    //printf("file   : %s\n", file_name);
    //printf("pattern: %s\n", pattern);

    if (doit) {
        //printf("DOIT\n");
        run_find(dir_name, pattern);
    }

    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;

}
