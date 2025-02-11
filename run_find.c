#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "clilib.h"
#include "strlib.h"
#include "wstrlib.h"
#include "pathlib.h"
#include "wclib.h"
#include "fslib.h"
#include "syslib.h"

static void _print_file_path(const char* path) {
    if (!path) {
        return;
    }
    printf("%s\n", path);
}

static void _print_file(lib_file_t* file) {
    if (!file) {
        return;
    }
    char* path = file->name ? lib_fs_get_real_path(file->name) : NULL;
    if (!path) {
        return;
    }
    _print_file_path(path);
    free(path);
}

static void _find_pattern(const char* dir_name, const char* pattern) {

    lib_file_t** files = NULL;
    lib_file_t* file = NULL;
    int count = lib_fs_scandir(dir_name, pattern, &files, LIB_SCANDIR_RECURSIVE, true);

    printf("count: %d\n", count);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            file = files[i];
            if (!file) {
                // error
                continue;
            }
            _print_file(file);
            //free(file);
            //lib_file_free(file);
        }
    }

    lib_files_free(files);
}

static void _find_item(const char* arg) {

    char* expression = lib_strdupuq(arg);
    char* dir_name  = NULL;
    char* pattern   = NULL;

    bool doit = false;

    // Get wildcard start index
    int wldc_index = lib_wc_get_start_index(expression);
    int path_index = 0;

    //printf(">> expression : %s\n", expression);
    //printf(">> wldc_index : %d\n", wldc_index);

    if (wldc_index >= 0) {
        
        //printf(">> mode: wildcard\n");

        // dir1/dir2/file*
        // dir1/dir2/file?

        // file*
        // file?

        path_index = lib_wc_get_path_index(wldc_index, expression);
        if (path_index >= 0) {

            // dir1/
            dir_name = lib_strndup(expression, path_index + 1);

            // dir2/file*
            // dir2/file?

            expression = expression + path_index + 1;
        } else {

            // .   - Nix
            // ./* - Win
            dir_name = lib_strdup(lib_fs_get_current_find_path());
        }

        pattern = expression;
        doit = true;

    } else {

        //printf(">> mode: path\n");

        char* path = lib_fs_get_real_path(expression);
        if (path) {
            _print_file_path(path);
            free(path);
        } else {
            dir_name = lib_fs_get_dir_name(expression);
            if (!dir_name || strcmp(dir_name, ".") == 0) {

                // Scandir in current directory for one file
                if (dir_name) {
                    free(dir_name);
                }
                dir_name = lib_strdup(lib_fs_get_current_find_path());
                pattern = expression;
                doit = true;

            } else {
                free(dir_name);
                dir_name = NULL;
            }
        }

    }

    //printf(">> directory  : %s\n", dir_name);
    //printf(">> expression : %s\n", expression);
    //printf(">> pattern    : %s\n", pattern);

    if (doit) {
        //printf(">> doit\n");
        _find_pattern(dir_name, pattern);
    }

    if (expression && path_index > 0) {
        expression = expression - path_index - 1;
    }

    free(dir_name);
    free(expression);

}

int run_find(int argc, char* argv[]) {
    lib_sys_locale_init();

    ////
    for (int i = 1; i < argc; i++) {
        _find_item(argv[i]);
    }
    ////

    lib_sys_locale_restore();
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-find path\n");
    fprintf(stderr, "       run-find pattern\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);
    return run_find(argc, argv);
}
