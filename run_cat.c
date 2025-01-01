#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "arglib.h"
#include "iolib.h"
#include "wstrlib.h"
#include "syslib.h"

static int _proc_file(const char* file_name) {
    char* data       = NULL;
    size_t data_size = 0;

    // read
    int retval = lib_io_read_all_bytes(file_name, &data, &data_size);
    if (retval < 0 || !data) {
        fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
        free(data);
        data      = NULL;
        data_size = 0;
        return -1;
    }

    // write
    for (int i = 0; i < data_size; i++) {
        printf("%c", data[i]);
    }

    free(data);

    return 0;
}

int run_cat(const char** file_names, size_t file_count) {

    if (!file_names) {
        return 1;
    }

    if (file_count == 0) {
        return 0;
    }

    lib_sys_locale_init();

    ////
    const char* file_name = NULL;
    for (int i = 0; i < file_count; i++) {

        file_name = file_names[i];
        if (!file_name) {
            // error
            continue;
        }

        _proc_file(file_name);

    }
    ////

    lib_sys_locale_restore();

    // char* total_data = NULL;
    // size_t total_size = 0;
    // int retval = lib_io_read_cat_bytes(file_names, file_count, &total_data, &total_size);
    // if (retval != 0) {
    //     fprintf(stderr, "%s: No such file or directory\n", prog_name);
    //     free(total_data);
    //     return 1;
    // }
    // if (!total_data) {
    //     fprintf(stderr, "%s: No data\n", prog_name);
    //     return 1;
    // }
    // lib_sys_locale_init();
    // for (int i = 0; i < total_size; i++) {
    //     printf("%c", total_data[i]);
    // }
    // lib_sys_locale_restore();

    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-cat file ...\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);

    int file_count = argc - 1;

    const char* file_names[file_count];
    for (int i = 1; i < argc; i++) {
        file_names[i - 1] = argv[i];
    }

    return run_cat(file_names, file_count);
}
