#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "iolib.h"

void print_usage() {
    printf("Usage: run-dump [-w 16|32] <file>\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }

    int width = DUMP_DEF_COL_COUNT;
    int show_text = DUMP_DEF_SHOW_TEXT;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "w:t")) != -1) {
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
        case 't':
            show_text = true;
            break;
        case '?':
            error = true;
            break;
        }
    }

    if (error) {
        print_usage();
        return 0;
    }

    char* file_name = argv[optind];

    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    dump_config_t* config = (dump_config_t*) malloc(sizeof(dump_config_t));
    config->col_count = width;
    config->show_text = show_text;

    dump_file(file_name, config);
}