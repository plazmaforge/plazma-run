#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "dumplib.h"
#include "iodef.h"

void usage() {
    fprintf(stderr, "Usage: run-dump [-w 16 | 32] [-a] file\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 1; // <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    int width = LIB_DUMP_DEF_COL_COUNT;
    bool show_text = false; //DUMP_DEF_SHOW_TEXT;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "w:a")) != -1) {
        switch (opt) {
        case 'w':
            if (optarg) {
                if (strcmp(optarg, "16") == 0) {
                    width = 16;
                } else if (strcmp(optarg, "32") == 0) {
                    width = 32;
                } else {
                    error = true;
                }
            } else {
                error = true;
            }
            break;
        case 'a':
            /// 'All' - segment, data, text
            show_text = true;
            break;
        case '?':
            error = true;
            break;
        case ':':
            error = true;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
        usage();
        return 0;
    }

    char* file_name = argv[optind];

    lib_io_buf_init();

    lib_dump_config_t* config = (lib_dump_config_t*) malloc(sizeof(lib_dump_config_t));
    config->col_count = width;
    config->show_text = show_text;

    lib_dump_file_def(file_name, config);
}