
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
#include "asklib.h"

void usage() {
    fprintf(stderr, "Usage: run-ask [-bil] text file \n");
}

int main(int argc, char* argv[]) {

    int min_arg = 2; // <text> <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    // Config
    bool binary_mode = false;
    bool find_first_only = false;
    bool ignore_case = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "bil")) != -1) {
        switch (opt) {
        case 'b':
            binary_mode = true;
            break;
        case 'i':
            ignore_case = true;
            break;
        case 'l':
            find_first_only = true;
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
        fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
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

    init_locale();

    ask_config* config = ask_config_new();
    config->binary_mode = binary_mode;
    config->find_first_only = find_first_only;
    config->ignore_case = ignore_case;
    config->print_file_name = true; // TODO: add optional

    int wildcard_index = lib_wc_get_wildcard_index(file_name);

    if (wildcard_index >= 0) {

        int path_index = lib_wc_get_wildcard_path_index(wildcard_index, file_name);
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
        fs_file_t** files = NULL;
        fs_file_t* file = NULL;
        int file_count = lib_fs_scandir(dir_name, pattern, &files, FS_SCANDIR_RECURSIVE, true);

        for (int i = 0; i < file_count; i++) {
            file = files[i];
            ask_find(files[i]->name, input, input_size, config);

            //free(file);
            //fs_file_free(file);
        }

        lib_fs_files_free(files);
                
        free(dir_name);
        free(config);
        restore_locale(); // Important for WIN32: The locale was changed for the terminal
        return 0;
    }

    ask_find(file_name, input, input_size, config);

    free(config);
    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;

}
