#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"
#include "run_htmllib.h"

int run_txt2html_data(lib_html_config_t* config, char* data, size_t size) {
    return run_html(config, data, size);    
}

int run_txt2html_file(lib_html_config_t* config, const char* file_name) {
    
    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    char* data  = NULL;
    size_t size = 0;
    int error = lib_io_read_all_bytes(file_name, &data, &size);
    if (error) {
        fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
        if (data) {
            free(data);
        }        
        return 1;
    }
    error = run_txt2html_data(config, data, size);
    if (data) {
        free(data);
    }
    return error;    
}

void usage() {
    fprintf(stderr, "Usage: run-txt2html -s <string> | <file>\n");
}

int main(int argc, char* argv[]) {

    lib_cli_prog_init(argv);

    if (lib_cli_not_arg(argc)) {
        usage();
        return 1;
    }

    int error = 0;
    int opt;
    int long_ind;

    bool flag_string        = false;
    char* data              = NULL;
    size_t size             = 0;
    const char* file_name   = NULL;

    // config
    const char* title       = NULL;
    const char* margin      = NULL;
    const char* font_name   = NULL;
    const char* font_weight = NULL;
    const char* font_size   = NULL;

    bool flag_title         = false;
    bool flag_margin        = false;
    bool flag_font_name     = false;
    bool flag_font_weight   = false;
    bool flag_font_size     = false;

    static lib_option long_options[] = {
          {"title",       optional_argument, 0, 1001},
          {"margin",      optional_argument, 0, 1002},
          {"font-name",   optional_argument, 0, 1003},
          {"font-weight", optional_argument, 0, 1004},
          {"font-size",   optional_argument, 0, 1005},
          {NULL,          0,                 0, 0 }
    };

    while ((opt = lib_getopt_long(argc, argv, "s:", long_options, &long_ind)) != -1) {

        switch (opt) {
        case 's':
            flag_string = true;
            data = optarg;
            break;

        case 1001: // title
            flag_title = true;
            title = optarg;
            break;
        case 1002: // margin
            flag_margin = true;
            margin = optarg;
            //fprintf(stderr, "margin-str: %s\n", optarg);
            //fprintf(stderr, "margin-int: %d\n", optarg ? atoi(optarg) : 0);
            break;
        case 1003: // font-name
            flag_font_name = true;
            font_name = optarg;
            break;
        case 1004: // font-weight
            flag_font_weight = true;
            font_weight = optarg;
            break;
        case 1005: // font-size
            flag_font_size = true;
            font_size = optarg;
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
        return 1;
    }

    if (flag_string) {
        optind--;
    }

    if (lib_cli_not_argmin(argc, optind, 1)) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 1;
    }

    error = 0;
    if (flag_string) {
        //BY_STRING
        if (!data) {
            fprintf(stderr, "%s: Input string is required\n", prog_name);
            error = 1;
        } else {
            size = strlen(data);
            if (size == 0) {
                fprintf(stderr, "%s: Input string is empty\n", prog_name);
                error = 1;
            }
        }
    } else {
        //BY_FILE
        if (lib_cli_not_argind(argc, optind)) {
            fprintf(stderr, "%s: File name is required\n", prog_name);
            error = 1;
        } else {
            file_name = argv[optind];
        }
    }

    if (error) {
        usage();
        return 1;
    }

    lib_html_config_t config;
    lib_html_init(&config);

    config.title       = title;
    config.margin      = flag_margin ? margin : LIB_HTML_MAARGIN;
    config.font_name   = flag_font_name ? font_name : LIB_HTML_FONT_NAME;
    config.font_weight = flag_font_weight ? font_weight : LIB_HTML_FONT_WEIGHT;
    config.font_size   = flag_font_size ? font_size : LIB_HTML_FONT_SIZE;

    error = 0;
    if (flag_string) {
        error = run_txt2html_data(&config, data, size);
    } else {
        error = run_txt2html_file(&config, file_name);
    }

    if (error) {
        return 1;
    }
    
    return 0;
}