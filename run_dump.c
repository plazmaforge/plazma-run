#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"
#include "dmplib.h"
#include "iodef.h"

int run_dump(lib_dmp_config_t* config, char* file_name) {
   //lib_io_buf_init();
   int retval = lib_dmp_dump_file_all(config, file_name);
   if (retval < 0) {
       fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
       return 1;
   }
   return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-dump [-w 16 | 32] [-a] file\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 1; // <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    // config
    int width = LIB_DMP_DEF_COL_COUNT;
    bool show_text = false; //DUMP_DEF_SHOW_TEXT;

    while ((opt = lib_getopt(argc, argv, "w:a")) != -1) {
        switch (opt) {
        case 'w':
            if (optarg) {
                if (strcmp(optarg, "16") == 0) {
                    width = 16;
                } else if (strcmp(optarg, "32") == 0) {
                    width = 32;
                } else {
                    error = 1;
                }
            } else {
                error = 1;
            }
            break;
        case 'a':
            /// 'All' - segment, data, text
            show_text = true;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }

    char* file_name = argv[optind];

    lib_dmp_config_t config;
    config.col_count = width;
    config.show_text = show_text;

    return run_dump(&config, file_name);
}