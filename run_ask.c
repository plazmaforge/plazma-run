#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "arglib.h"
#include "strlib.h"
#include "wstrlib.h"
#include "pathlib.h"
#include "wclib.h"
#include "fslib.h"
#include "syslib.h"
#include "asklib.h"

int _find_file(lib_ask_config_t* config, char* file_name, char* input, size_t input_size) {
    int retval = lib_ask_find(config, file_name, input, input_size);
    if (retval != 0) {
        fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
        retval = 1;
    }
    return retval;
}

int _find_pattern(lib_ask_config_t* config, char* file_name, char* input, size_t input_size) {
    int wldc_index = lib_wc_get_wildcard_index(file_name);
    if (wldc_index < 0) {
        return -1;
    }
    int path_index = lib_wc_get_wildcard_path_index(wldc_index, file_name);
    char* dir_name = NULL;
    if (path_index >= 0) {
        dir_name = lib_strndup(file_name, path_index + 1);
        file_name = file_name + path_index + 1;
    } else {
        dir_name = lib_strdup(lib_fs_get_current_find_path());
    }

    //printf("dir  : %s\n", dir_name);
    //printf("file : %s\n", file_name);

    char* pattern = file_name;
    lib_fs_file_t** files = NULL;
    lib_fs_file_t* file = NULL;
    int count = lib_fs_scandir(dir_name, pattern, &files, LIB_FS_SCANDIR_RECURSIVE, true);
    if (count <= 0) {
        lib_fs_files_free(files);
        free(dir_name);
        return count;
    }

    int retval = 0;
    for (int i = 0; i < count; i++) {
        file = files[i];
        if (_find_file(config, files[i]->name, input, input_size) != 0) {
            retval = 1;
        }

        //free(file);
        //fs_file_free(file);
    }

    lib_fs_files_free(files);                
    free(dir_name);

    return retval;
}

int run_ask(lib_ask_config_t* config, char* file_name, char* input, size_t input_size) {

    lib_sys_locale_init();

    int retval = 0;
    if (lib_wc_is_wildcard_pattern(file_name)) {
        retval = _find_pattern(config, file_name, input, input_size);
    } else {
        retval = _find_file(config, file_name, input, input_size);
    }

    lib_sys_locale_restore();
    return retval;
}

void usage() {
    fprintf(stderr, "Usage: run-ask [-bil] text file \n");
}

int main(int argc, char* argv[]) {

    int min_arg = 2; // <text> <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    // Config
    bool bin_mode        = false;
    bool find_first_only = false;
    bool ignore_case     = false;

    while ((opt = lib_getopt(argc, argv, "bil")) != -1) {
        switch (opt) {
        case 'b':
            bin_mode = true;
            break;
        case 'i':
            ignore_case = true;
            break;
        case 'l':
            find_first_only = true;
            break;
        case '?':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind != min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }
                                        
    char* input = lib_strdupuq(argv[optind]);
    char* file_name = lib_strdupuq(argv[++optind]);

    //printf("input: %s\n", input);
    //printf("file : %s\n", file_name);

    size_t input_size = strlen(input);
    if (input_size == 0) {
        //printf("Input is empty\n");
        free(input);
        free(file_name);
        return 0;
    }

    lib_ask_config_t config;
    lib_ask_config_init(&config);

    config.bin_mode        = bin_mode;
    config.find_first_only = find_first_only;
    config.ignore_case     = ignore_case;
    config.print_file_name = true; // TODO: add optional

    int retval = run_ask(&config, file_name, input, input_size);

    free(input);
    free(file_name);

    return retval;
}
